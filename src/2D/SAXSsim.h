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
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include <cmath>
#include <chrono>
#include "PixelCenterDistances.h"
#ifdef ENABLE_PARALLEL
#include <omp.h>
#endif

class SAXSsim
{
public:
    using index_pair        = std::array<unsigned int, 2>;
    using index_pair_vector = std::vector<index_pair>;
    using intensities_vector =  std::vector<std::vector<double> > ;

    SAXSsim() = default;
    SAXSsim(const std::string imgName, std::string outputPlotName = "",
            std::string save_dist = "", std::string load_dist = "", int num_threads = 1);
    virtual ~SAXSsim ();
    cv::Mat & Read(const std::string &imgName);
    void SavePlot(const std::string & fname);
    void ShowPlot(const std::string & resultfile, double image_resolution);
    cv::Mat & DFT(cv::Mat & realSpaceMatrix);
    void Show();
    void SaveImage(cv::Mat & img, std::string & output);
    void PixelDistances(const cv::Mat &dualSpaceMatrix);
    void InitializeDistancesIndexes();
    PixelCenterDistances distances_indexes;

    intensities_vector & IntensityFromDistanceVector();
    intensities_vector intensities_at_distance;
    std::vector<double>&  MeanIntensities();
    std::vector<double> intensities_mean;
#ifdef ENABLE_PARALLEL
    intensities_vector & ParallelIntensityFromDistanceVector();
#endif
    void InitializeSizeMembers(const cv::Mat & dftMat);
    std::pair<unsigned int,unsigned int> mid_size;
    std::pair<unsigned int,unsigned int> dft_size;
    std::pair<bool, bool> even_flag;
    std::pair<double,double> origin;
    unsigned int d_assigned_max{0};
    unsigned int xi_begin{0}, xi_end{0}, yi_begin{0}, yi_end{0};

    cv::Mat I_;
    cv::Mat dftMat_;
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
protected:
    index_pair_vector SimetricIndexPairsFromIndexPair(const index_pair &);
    void SimetricIndexes();
    void ExtraIndexOddX();
    void ExtraIndexOddY();
    void ExtraIndexOddBoth();

    template<typename T = double>
    inline T Modulo (const T &a, const T& b){
        return sqrt(a*a + b*b);
    }
};
#endif
