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
#include <string>
class SAXSsim
{
public:
    SAXSsim() = default;
    SAXSsim(std::string imgName);
    virtual ~SAXSsim ();
    void Read();
    cv::Mat & FFT();
    void Show();
private:
    std::string inputName_;
    cv::Mat I_;
    cv::Mat F_;
};
#endif
