/* SAXSsim, Apply to a microscopy image, returning a I-q data set, allowing comparisson with Small Angle X-ray Scattering experiments.
Copyright (C) 2015 Pablo Hernandez-Cerdan

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SAXSsim_H_
#define SAXSsim_H_

#include "opencv2/core/core.hpp"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkSCIFIOImageIO.h"
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include <cmath>
#include <chrono>
#ifdef ENABLE_PARALLEL
#include <omp.h>
#endif

class SAXSsim
{
public:
    // ITK Image Type
    const static unsigned int  Dimension = 2;
    typedef unsigned int       InputPixelType;
    typedef double             RealPixelType;
    typedef itk::Image< InputPixelType, Dimension> InputImageType;
    typedef itk::Image< RealPixelType, Dimension>   RealImageType;
    typedef itk::Image< std::complex<RealPixelType>, Dimension> DFTImageType;

    using intensities_vector =  std::vector<std::vector<double> > ;

    SAXSsim() = default;
    SAXSsim(const std::string imgName, std::string outputPlotName = "",
            std::string save_dist = "", std::string load_dist = "", int num_threads = 1);
    virtual ~SAXSsim ();
    InputImageType* & Read(const std::string &imgName);
    DFTImageType* & DFT();
    void SavePlot(const std::string & fname);
    void ShowPlot(const std::string & resultfile, double image_resolution);
    void Show();
    void SaveImage(cv::Mat & img, std::string & output);

    intensities_vector & IntensityFromDistanceVector();
    intensities_vector intensities_at_distance;
    std::vector<double>&  MeanIntensities();
    std::vector<double> intensities_mean;
#ifdef ENABLE_PARALLEL
    intensities_vector & ParallelIntensityFromDistanceVector();
#endif
    void InitializeSizeMembers(const cv::Mat & dftMat);
    std::pair< int, int> mid_size;
    std::pair< int, int> dft_size;
    std::pair<bool, bool> even_flag;
    std::pair<double,double> origin;
    int d_assigned_max{0};
    int xi_begin{0}, xi_end{0}, yi_begin{0}, yi_end{0};

    InputImageType* I_;
    DFTImageType* dftMat_;
    RealImageType* fftModulus_;
    const std::string inputName_;
    int num_threads_{1};
    /**Structure to save parameters, set at constructor. */
    struct Input{
        std::string img;
        std::string outPlot;
        std::string saveIndices;
        std::string loadIndices;
        int threads{1};
    };
    Input input;
    void logDFT(cv::Mat &D);
protected:
    void CheckEqualDimension();
    bool log_flag_{false};

    template<typename T = double>
    inline T Modulo (const T &a, const T& b){
        return sqrt(a*a + b*b);
    }
};
#endif
