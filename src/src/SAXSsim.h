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
namespace SAXSsimUtils {
template<typename T = double>
T modulo (const T &a, const T& b){
    return sqrt(a*a + b*b);
};
}
class SAXSsim
{
public:
    using index_pair        = std::array<unsigned int, 2>;
    using index_pair_vector = std::vector<index_pair>;
    using intensities_vector =  std::vector<std::vector<double> > ;

    SAXSsim() = default;
    SAXSsim(const std::string imgName, std::string outputPlotName = "");
    virtual ~SAXSsim ();
    cv::Mat & Read(const std::string &imgName);
    void Save(const std::string & fname, const std::string & relativeOutputFolder = "./");
    cv::Mat & DFT(cv::Mat & realSpaceMatrix);
    void Show();

    void PixelDistances(const cv::Mat &dualSpaceMatrix);
    index_pair_vector SimetricIndexPairsFromIndexPair(const index_pair &);
    intensities_vector & IntensityFromDistanceVector(cv::Mat &);
    std::vector<double>&  MeanIntensities();

    std::vector<index_pair_vector> distances_indexes;
    intensities_vector intensities_at_distance;
    std::vector<double> intensities_mean;

    std::pair<unsigned int,unsigned int> mid_size;
    std::pair<unsigned int,unsigned int> dft_size;
    std::pair<bool, bool> even_flag;
private:
    const std::string inputName_;
    cv::Mat I_;
    cv::Mat dftMat_;
};
#endif
