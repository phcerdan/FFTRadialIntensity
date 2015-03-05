#include "SAXSsim.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdexcept>
#include <iostream>
using namespace cv;
using namespace std;
using namespace SAXSsimUtils;
SAXSsim::SAXSsim(string inputName) :
    inputName_{inputName}
{
    Read(inputName_);
    DFT(I_);
    // Show();
}

SAXSsim::~SAXSsim(){}

Mat& SAXSsim::Read(string inputName){
    I_ = imread(inputName.c_str(), IMREAD_GRAYSCALE);
    if(I_.empty()) throw
        runtime_error("Read failed for image: " + inputName + " .Empty matrix");
    return I_;
}

Mat& SAXSsim::DFT(Mat &I){
    // Create a new padded image with borders added to original image.
    Mat padded;
    int m = getOptimalDFTSize( I.rows );
    int n = getOptimalDFTSize( I.cols );
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    // The result of dft is complex:
    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))
    split(complexI, planes);
    Mat magI;
    magnitude(planes[0], planes[1], magI);
    // Switch to logscale.
    magI += Scalar::all(1);
    log(magI,magI);
    //TODO is that & a bit operator to crop back the image?
    // magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
    int cx = magI.cols/2;
    int cy = magI.rows/2;

    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
    // swap quadrants (Top-Left with Bottom-Right)
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    // swap quadrant (Top-Right with Bottom-Left)
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    dftMat_ = magI.clone();
    normalize(dftMat_, dftMat_, 0, 1, NORM_MINMAX, -1, Mat());
    return dftMat_;
}
void SAXSsim::PixelDistances(Mat &dftMat){
    mid_size  = make_pair (dftMat.cols/2, dftMat.rows/2);
    dft_size  = make_pair (dftMat.cols, dftMat.rows);
    even_flag = make_pair (dftMat.cols % 2 == 0, dftMat.rows % 2 == 0);
    double origin_x = even_flag.first ? 0.0 : 0.5;
    double origin_y = even_flag.second ? 0.0 : 0.5;
    double distance_max = modulo<double>(mid_size.first - origin_x, mid_size.second - origin_y);
    unsigned d_assigned_max = static_cast<unsigned int>(distance_max) + 1;

    //Initialize distances_indexes with empty vectors from d = 0(1 value) until d_assigned_max + 1.
    for( unsigned int d = 0; d!=d_assigned_max + 1; d++){
        distances_indexes.push_back(index_pair_vector{});
    }

    double center_distance_prox{0}, x_prox{0}, y_prox{0},
           center_distance_far{0}, x_far{0}, y_far{0};
    unsigned int d_assigned{0}, d_prox{0}, d_far{0};

    unsigned int xi_begin = even_flag.first ? 0 : 1,
                 xi_end = even_flag.first ? mid_size.first : mid_size.first +1 ,
                 yi_begin = even_flag.second ? 0 : 1,
                 yi_end = even_flag.second ? mid_size.second : mid_size.second +1 ;
    double xcp{0},ycp{0}, slope{0};

    //Calculate the distance from center of pixel to center of image (0,0);
    for(unsigned int xi = xi_begin; xi!=xi_end; xi++){
        for(unsigned int yi = yi_begin; yi!=yi_end; yi++){
            xcp = even_flag.first? xi + 0.5 : xi ;
            ycp = even_flag.second? yi + 0.5 : yi ;
            slope = ycp / xcp; // Cannot be 0.

            if(slope >= 1.0){
                y_prox = yi - origin_y;
                x_prox = y_prox / slope;
                y_far = yi + 1 -origin_y;
                x_far = y_far / slope;
            } else {
                x_prox = xi - origin_x;
                y_prox = slope * x_prox ;
                x_far = xi + 1 -origin_x;
                y_far = slope *x_far;
            }

            center_distance_prox = modulo<double>(x_prox, y_prox);
            center_distance_far = modulo<double> (x_far, y_far);
            d_prox              = static_cast<unsigned int> (center_distance_prox);
            d_far               = static_cast<unsigned int> (center_distance_far);
            if (center_distance_far - trunc(center_distance_far) < 5*std::numeric_limits<double>::epsilon())
                d_assigned          = d_far - 1;
            else
                d_assigned          = d_far;

            std::cout<<"center_distance_prox " << center_distance_prox<<" center_distance_far"<< center_distance_far<< " d_assigned: " << d_assigned << " " << xi << " " << yi << std::endl;
            index_pair_vector v = SimetricIndexPairsFromIndexPair(index_pair{xi,yi});
            auto it             = distances_indexes[d_assigned].end();
            distances_indexes[d_assigned].insert(it, v.begin(), v.end());

            if (d_far - d_prox == 2 && d_assigned != d_far - 1){
                unsigned int d_extra = d_far - 1;
                index_pair_vector v = SimetricIndexPairsFromIndexPair(index_pair{xi,yi});
                auto it             = distances_indexes[d_extra].end();
                distances_indexes[d_extra].insert(it, v.begin(), v.end());
            std::cout<<" d_extra: " << d_extra << " " << xi << " " << yi << std::endl;
            }
        }
    }

    // If any of the matrix dimensions d are odd, get the skipped indexes for d[0]
    // odd in d=x
    if (!even_flag.first){
        index_pair first_pixel{mid_size.second, mid_size.first};
        for (unsigned int di = yi_begin; di!= yi_end; di++){
            y_prox     = di - origin_y;
            if(y_prox < 0.0) y_prox = 0.0;
            if ( y_prox - trunc(y_prox) < 5*std::numeric_limits<double>::epsilon())
                d_assigned = static_cast<unsigned int>(y_prox);
            else
                d_assigned  = static_cast<unsigned int>(y_prox) + 1;
            distances_indexes[d_assigned].push_back(index_pair{mid_size.second, mid_size.first + di});
            distances_indexes[d_assigned].push_back(index_pair{mid_size.second, dft_size.first - 1 - mid_size.first - di});
        }
    }
    // odd in d=y
    if (!even_flag.second){
        index_pair first_pixel{mid_size.first, mid_size.second};
        for (unsigned int di = xi_begin; di!= xi_end; di++){
            x_prox     = di - origin_x;
            if(x_prox < 0.0) x_prox = 0.0;
            if ( x_prox - trunc(x_prox) < 5*std::numeric_limits<double>::epsilon())
                d_assigned = static_cast<unsigned int>(x_prox);
            else
                d_assigned  = static_cast<unsigned int>(x_prox) + 1;
            distances_indexes[d_assigned].push_back(index_pair{mid_size.second + di, mid_size.first });
            distances_indexes[d_assigned].push_back(index_pair{dft_size.second - 1 - mid_size.first - di, mid_size.first });
        }
    }
    // odd in x and y;
    // Get the center pixel.
    if (!even_flag.first && !even_flag.second){
        distances_indexes[0].push_back(index_pair{mid_size.first,mid_size.second});
    }
}

SAXSsim::index_pair_vector SAXSsim::SimetricIndexPairsFromIndexPair(const SAXSsim::index_pair &ind_pair){
    unsigned int x1     = mid_size.first + ind_pair[0];
    unsigned int y1     = mid_size.second + ind_pair[1];
    index_pair p1{x1,y1};
    index_pair p2{x1, dft_size.second -1 -y1};
    index_pair p3{dft_size.first -1 - x1, y1};
    index_pair p4{ dft_size.first -1 - x1, dft_size.second -1 - y1};

    return index_pair_vector{p1, p2, p3, p4};
}

histo::HistoB<double>  SAXSsim::Scatter(Mat& dftMat){
    std::vector<double> d{1,1,1,1,40,450, 43,20, 20, 21, 300};
    std::vector<double> b{-100, 1, 100,200, 300,400, 500};
    histo::HistoB<double> h(d, b);
    return h;
}

SAXSsim::intensities_vector & SAXSsim::IntensityFromDistanceVector( cv::Mat & dftMat){
    unsigned int d_max = this->distances_indexes.size();
    intensities_at_distance.resize(d_max);
    double *p;
    double I{0}, d_aprox{0};
    unsigned int  d{0} ;

    auto it_begin = distances_indexes[0].begin(),
         it_end = distances_indexes[0].end(),
         it_found = it_end;
    for(unsigned int y = 0; y!=dftMat.rows; y++){
        p = dftMat.ptr<double>(y);
        for(unsigned int x = 0; x!=dftMat.cols; x++){

            I = p[x];
            d_aprox = SAXSsimUtils::modulo<double>(x - mid_size.first, y - mid_size.second);
            // Search index in d= [d_aprox -1, d_aprox, d_aprox +1 ]
            d = static_cast<unsigned int>(d_aprox);
            it_begin = distances_indexes[d].begin();
            it_end = distances_indexes[d].end();
            it_found = find(it_begin, it_end, index_pair{x,y});
            if (it_found!=it_end) intensities_at_distance[d].push_back(I);

            if (d!= 0){
                d--;
                it_begin = distances_indexes[d].begin();
                it_end = distances_indexes[d].end();
                it_found = find(it_begin, it_end, index_pair{x,y});
                if (it_found!=it_end) intensities_at_distance[d].push_back(I);
            }
            if (d!= d_max){
                d++;
                it_begin = distances_indexes[d].begin();
                it_end = distances_indexes[d].end();
                it_found = find(it_begin, it_end, index_pair{x,y});
                if (it_found!=it_end) intensities_at_distance[d].push_back(I);
            }
        }
    }
    return intensities_at_distance;
}
void SAXSsim::Show(){
    imshow("Input Image"  , I_   );
    imshow("DFT:Magnitude", dftMat_);
    waitKey();
}
