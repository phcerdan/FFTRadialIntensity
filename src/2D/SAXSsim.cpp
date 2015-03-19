#include "SAXSsim.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdexcept>
#include <iostream>
#include <fstream>
// #include <cereal/archives/portable_binary.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
using serialize_input_type = boost::archive::text_iarchive;
using serialize_output_type = boost::archive::text_oarchive;
// using serialize_input_type = cereal::PortableBinaryInputArchive;
// using serialize_output_type = cereal::PortableBinaryOutputArchive;
// #include <cereal/archives/json.hpp>
// using cereal_input_type = cereal::JSONInputArchive;
// using cereal_output_type = cereal::JSONOutputArchive;

using namespace cv;
using namespace std;
SAXSsim::SAXSsim(const string inputName, string outputName, string save_dist, string load_dist) :
    inputName_{inputName}
{

    Read(inputName_);
    DFT(I_);
    if(load_dist ==""){

        cout << "Computing new set of distances_index..." << endl;
        PixelDistances(dftMat_);

    } else {

        PixelCenterDistances loaded_data;
        {
            ifstream sinp(load_dist);
            serialize_input_type iarchive(sinp);
            iarchive >> loaded_data ;
        }

        if(loaded_data.Nx == dft_size.first && loaded_data.Ny == dft_size.second){

            distances_indexes = loaded_data;
            InitializeSizeMembers(dftMat_);

        } else
            throw std::runtime_error("Size of input image:[ " + std::to_string(dft_size.first) + ", "
                    + std::to_string(dft_size.second) +  " ] is different from loaded data: [ "
                    + std::to_string(loaded_data.Nx) + ", " + std::to_string(loaded_data.Ny) + " ]\n"
                    "Change the data to load, resize the image, or generate new distances_indexes"
                    "running the executable without -l or --load_dist option. "
             );
    }

    if(save_dist == "")
        save_dist = "./serialization/x"+ std::to_string(dft_size.first) + "_y" + std::to_string(dft_size.second);

    {
        boost::filesystem::path opath{save_dist};
        boost::filesystem::create_directories(opath.parent_path());
        ofstream sout(save_dist);
        serialize_output_type oarchive(sout);
        oarchive << distances_indexes;
    }

    cout << "Computing Intensity..." << endl;
    IntensityFromDistanceVector();
    MeanIntensities();
    SavePlot(outputName);
    // Show();
}

SAXSsim::~SAXSsim(){}

Mat& SAXSsim::Read(const string &inputName){
    I_ = imread(inputName.c_str(), IMREAD_GRAYSCALE);
    if(I_.empty()) throw
        runtime_error("Read failed for image: " + inputName + " .Empty matrix");
    return I_;
}

void SAXSsim::SavePlot(const string & fname, const string & relativeOutputFolder /* = "./"  */){

    boost::filesystem::path dir(relativeOutputFolder);
    boost::filesystem::path path(dir/(fname +".plot"));
    if (relativeOutputFolder != "./"){
        boost::filesystem::create_directories(dir);
    }
    std::ofstream output_file (path.string()); // delete everything inside the file(default)

    if (!output_file.is_open()) {
        perror( ("Error creating IvsQ file in " + path.string() ).c_str());
    }

    for (unsigned long long i = 0; i!=this->distances_indexes.ind.size() - 1; i++ ){
        output_file << i  << " " << this->intensities_mean[i] << std::endl;
    }

    if(output_file.bad()){
        perror( ("Error saving IvsQ file in " + path.string() ).c_str());
    }
    output_file.close();
}

Mat& SAXSsim::DFT(Mat &I){
    // Create a new padded image with borders added to original image.
    // Mat padded;
    // int m = getOptimalDFTSize( I.rows );
    // int n = getOptimalDFTSize( I.cols );
    // copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    // The result of dft is complex:
    // Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat planes[] = {Mat_<float>(I), Mat::zeros(I.size(), CV_32F)};
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
    dft_size  = make_pair (dftMat_.cols, dftMat_.rows);
    return dftMat_;
}

void SAXSsim::InitializeSizeMembers(const cv::Mat & dftMat){

    mid_size  = make_pair (dftMat.cols/2, dftMat.rows/2);
    dft_size  = make_pair (dftMat.cols, dftMat.rows);
    even_flag = make_pair (dftMat.cols % 2 == 0, dftMat.rows % 2 == 0);
    double origin_x = even_flag.first ? 0.0 : 0.5;
    double origin_y = even_flag.second ? 0.0 : 0.5;
    origin = make_pair(origin_x, origin_y);

    double distance_max = Modulo<double>(mid_size.first - origin.first, mid_size.second - origin.second);
    d_assigned_max = static_cast<unsigned int>(distance_max) + 2;

    xi_begin = even_flag.first ? 0 : 1;
    xi_end = even_flag.first ? mid_size.first : mid_size.first +1;
    yi_begin = even_flag.second ? 0 : 1;
    yi_end = even_flag.second ? mid_size.second : mid_size.second +1 ;
}

void SAXSsim::InitializeDistancesIndexes(){
    //Initialize distances_indexes with empty vectors from d = 0(1 value) until d_assigned_max.
    index_pair_vector empty_ipv{};
    for( unsigned int d = 0; d!=d_assigned_max; d++){
        distances_indexes.ind.push_back(empty_ipv);
    }
    distances_indexes.Nx  = dft_size.first;
    distances_indexes.Ny  = dft_size.second;
}

void SAXSsim::SimetricIndexes(){

    double center_distance_prox{0}, x_prox{0}, y_prox{0},
           center_distance_far{0}, x_far{0}, y_far{0};
    unsigned int d_assigned{0}, d_prox{0}, d_far{0};

    double xcp{0},ycp{0}, slope{0};

    //Calculate the distance from center of pixel to center of image (0,0);
    index_pair ipair{};
    for(unsigned int xi = xi_begin; xi!=xi_end; xi++){
        for(unsigned int yi = yi_begin; yi!=yi_end; yi++){
            xcp = even_flag.first? xi + 0.5 : xi ;
            ycp = even_flag.second? yi + 0.5 : yi ;
            slope = ycp / xcp; // Cannot be 0.

            if(slope >= 1.0){
                y_prox = yi - origin.second;
                x_prox = y_prox / slope;
                y_far = yi + 1 -origin.second;
                x_far = y_far / slope;
            } else {
                x_prox = xi - origin.first;
                y_prox = slope * x_prox ;
                x_far = xi + 1 -origin.first;
                y_far = slope *x_far;
            }

            center_distance_prox = Modulo<double>(x_prox, y_prox);
            center_distance_far = Modulo<double> (x_far, y_far);
            d_prox              = static_cast<unsigned int> (center_distance_prox);
            d_far               = static_cast<unsigned int> (center_distance_far);
            if (center_distance_far - trunc(center_distance_far) < 5*std::numeric_limits<double>::epsilon())
                d_assigned          = d_far - 1;
            else
                d_assigned          = d_far;

            // std::cout<<"center_distance_prox " << center_distance_prox<<" center_distance_far"<< center_distance_far<< " d_assigned: " << d_assigned << " " << xi << " " << yi << std::endl;
            ipair[0] = xi; ipair[1] =yi;
            index_pair_vector v = SimetricIndexPairsFromIndexPair(ipair);
            auto it             = distances_indexes.ind[d_assigned].end();
            distances_indexes.ind[d_assigned].insert(it, v.begin(), v.end());

            if (d_far - d_prox == 2 && d_assigned != d_far - 1){
                unsigned int d_extra = d_far - 1;
                index_pair_vector v = SimetricIndexPairsFromIndexPair(ipair);
                auto it             = distances_indexes.ind[d_extra].end();
                distances_indexes.ind[d_extra].insert(it, v.begin(), v.end());
            // std::cout<<" d_extra: " << d_extra << " " << xi << " " << yi << std::endl;
            }
        }
    }
}

void SAXSsim::ExtraIndexOddX(){
    double y_prox{0};
    unsigned int d_assigned{0};
    index_pair ipair{};

    for (unsigned int di = yi_begin; di!= yi_end; di++){
        y_prox     = di - origin.second;
        if(y_prox < 0.0) y_prox = 0.0;
        if ( y_prox - trunc(y_prox) < 5*std::numeric_limits<double>::epsilon())
            d_assigned = static_cast<unsigned int>(y_prox);
        else
            d_assigned  = static_cast<unsigned int>(y_prox) + 1;

        ipair[0] = mid_size.second;
        ipair[1] = mid_size.first + di;
        distances_indexes.ind[d_assigned].push_back(ipair);
        ipair[0] = mid_size.second;
        ipair[1] = dft_size.first - 1  - mid_size.first - di;
        distances_indexes.ind[d_assigned].push_back(ipair);
    }
}

void SAXSsim::ExtraIndexOddY(){

    double x_prox{0};
    unsigned int d_assigned{0};
    index_pair ipair{};

    for (unsigned int di = xi_begin; di!= xi_end; di++){
        x_prox     = di - origin.first;
        if(x_prox < 0.0) x_prox = 0.0;
        if ( x_prox - trunc(x_prox) < 5*std::numeric_limits<double>::epsilon())
            d_assigned = static_cast<unsigned int>(x_prox);
        else
            d_assigned  = static_cast<unsigned int>(x_prox) + 1;
        ipair[0] = mid_size.second + di;
        ipair[1] = mid_size.first ;
        distances_indexes.ind[d_assigned].push_back(ipair);
        ipair[0] = dft_size.second -1 -mid_size.second - di;
        ipair[1] = mid_size.first ;
        distances_indexes.ind[d_assigned].push_back(ipair);
    }
}

void SAXSsim::ExtraIndexOddBoth(){
    index_pair ipair{};
    ipair[0] = mid_size.first ;
    ipair[1] = mid_size.second ;
    distances_indexes.ind[0].push_back(ipair);
}

void SAXSsim::PixelDistances(const Mat &dftMat){
    InitializeSizeMembers(dftMat);
    InitializeDistancesIndexes();

    SimetricIndexes();

    if (!even_flag.first)  ExtraIndexOddX();
    if (!even_flag.second) ExtraIndexOddY();
    if (!even_flag.first && !even_flag.second) ExtraIndexOddBoth();

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

std::vector<double> & SAXSsim::MeanIntensities(){
    unsigned int d{0};
    double mean{0};
    intensities_mean.resize(distances_indexes.ind.size());
    for( auto & dv : intensities_at_distance){
        if (dv.size() != 0) {
            mean = 0.0;
            for (auto & iv : dv ){
               mean+=iv;
            }
            mean = mean / static_cast<double> (dv.size());
            intensities_mean[d] = mean;
        }
        d++;
    }
    return intensities_mean;
}

SAXSsim::intensities_vector & SAXSsim::IntensityFromDistanceVector(){
    intensities_at_distance.resize(distances_indexes.ind.size());
    unsigned int d_max = intensities_at_distance.size() - 1;

    float *p;
    double I{0}, d_aprox{0};
    unsigned int  d{0} ;

    auto it_begin = distances_indexes.ind[0].begin(),
         it_end = distances_indexes.ind[0].end(),
         it_found = it_end;
    // unsigned int debug_counts{0};
    index_pair ipair{};
    pair<double,double> d_correction = make_pair(mid_size.first + origin.first,
            mid_size.second + origin.second);
    for(unsigned int y = 0; y!=distances_indexes.Ny; y++){
        p = dftMat_.ptr<float>(y);
        for(unsigned int x = 0; x!=distances_indexes.Nx; x++){

            I = p[x];
            // d_aprox = Modulo<double>(static_cast<double>(x) - mid_size.first, static_cast<double>(y) - mid_size.second);
            d_aprox = Modulo<double>(static_cast<double>(x) - d_correction.first, static_cast<double>(y) - d_correction.second);
            // Search index in d= [d_aprox -1, d_aprox, d_aprox +1 ]
            d = static_cast<unsigned int>(d_aprox);
            it_begin = distances_indexes.ind[d].begin();
            it_end = distances_indexes.ind[d].end();
            ipair[0] = x;
            ipair[1] = y;
            it_found = find(it_begin, it_end, ipair);
            if (it_found!=it_end) intensities_at_distance[d].push_back(I);

            if (d!= 0){
                d--;
                it_begin = distances_indexes.ind[d].begin();
                it_end = distances_indexes.ind[d].end();
                it_found = find(it_begin, it_end, ipair);
                if (it_found!=it_end) intensities_at_distance[d].push_back(I);
                d++; //Restore old d for next step
            }
            if (d!= d_max){
                d++;
                it_begin = distances_indexes.ind[d].begin();
                it_end = distances_indexes.ind[d].end();
                it_found = find(it_begin, it_end, ipair);
                if (it_found!=it_end) intensities_at_distance[d].push_back(I);
            }
            // debug_counts++;
        }
    }
    // cout << "debug_counts: " << debug_counts << endl;
    return intensities_at_distance;
}

void SAXSsim::Show(){
    imshow("Input Image"  , I_   );
    normalize(dftMat_, dftMat_, 0, 1, NORM_MINMAX, -1, Mat());
    imshow("DFT:Magnitude", dftMat_);
    waitKey(0);
    destroyWindow("Input Image");
    destroyWindow("DFT:Magnitude");
}
