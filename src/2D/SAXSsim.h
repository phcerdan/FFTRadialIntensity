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
    // ITK typedefs
    const static unsigned int  Dimension = 2;
    typedef unsigned int       InputPixelType;
    typedef double             RealPixelType;
    typedef itk::Image< InputPixelType, Dimension> InputImageType;
    typedef itk::Image< RealPixelType, Dimension>   RealImageType;
    typedef itk::Image< std::complex<RealPixelType>, Dimension> ComplexImageType;
    typedef InputImageType::Pointer InputTypeP;
    typedef RealImageType::Pointer RealTypeP;
    typedef ComplexImageType::Pointer ComplexTypeP;

    using Intensities =  std::vector<std::vector<double> > ;

public:
    SAXSsim() = default;
    SAXSsim(const std::string imgName, std::string outputPlotName = "",
             int num_threads = 1);
    virtual ~SAXSsim ();
    InputTypeP  Read(const std::string &imgName);
    ComplexTypeP  & FFT();
    RealTypeP & FFTModulusSquare(const ComplexTypeP & D);
    RealTypeP LogFFTModulusSquare(const RealTypeP & modulo);
    void SaveIntensityProfile(const std::string & fname);
    void GeneratePDF(const std::string & resultfile, double image_resolution);
    void WriteFFTModulus( const std::string &outputFilename);

    Intensities & ComputeRadialIntensity();
#ifdef ENABLE_PARALLEL
    Intensities & ParallelComputeRadialIntensity();
#endif
    Intensities intensities_;
    std::vector<double>&  MeanIntensities();
    std::vector<double> intensitiesMean_;

    InputTypeP inputImg_;
    ComplexTypeP fftImg_;
    RealTypeP fftModulusSquare_;

    /**Structure to save input parameters, set at constructor. */
    struct InputParameters{
        std::string img;
        std::string outPlot;
        int threads{1};
    };
    InputParameters input;

    std::pair< int, int> midSize_;
    std::pair< int, int> imgSize_;
    std::pair<bool, bool> evenFlag_;
    int fMax_{0};
protected:
    void CheckEqualDimension();
    void InitializeSizeMembers();

    const std::string inputName_;
    int numThreads_{1};

    template<typename T = double>
    inline T Modulo (const T &a, const T& b){
        return sqrt(a*a + b*b);
    }
};
#endif
