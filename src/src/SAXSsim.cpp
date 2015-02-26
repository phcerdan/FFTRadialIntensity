#include "SAXSsim.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdexcept>
#include <iostream>
using namespace cv;
using namespace std;

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
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
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

histo::Histo<double> & SAXSsim::Scatter(Mat& dftMat){
    // return histogram;
}

void SAXSsim::Show(){
    imshow("Input Image"  , I_   );
    imshow("DFT:Magnitude", dftMat_);
    waitKey();
}
