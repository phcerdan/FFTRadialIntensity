/**
 FFT From Image. Apply to a microscopy image, returning a I-q data set,
 allowing comparisson with Small Angle X-ray Scattering experiments.
 Copyright © 2015 Pablo Hernandez-Cerdan

 This library is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include "prog_options.h"
#include "image_functions.h"
#include "radial_intensity.h"
#include "saxs_sim_functional.h"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

int main(int argc, char* argv[]){

    try {
        auto option_map = program_options(argc, argv);
        string input = option_map["input_img"].as<string>();
        string output = option_map["output"].as<string>();
        //TODO allow prog options to accept different dimensions
        using ImageType = itk::Image<float, 2>;
        const fs::path ipath{input};
        auto intensities_histogram_meta = radial_intensity::ComputeRadialIntensitiyFromImage<ImageType>(fs::absolute(ipath).string());
        const auto & intensities = std::get<0>(intensities_histogram_meta);
        // auto & histo = std::get<1>(intensities_histogram_meta);
        auto & meta = std::get<2>(intensities_histogram_meta);
        auto average_intensities = radial_intensity::AverageRadialFrequencyIntensities(intensities);
        std::string img_basename = ipath.stem().string();
        // Change meta.name to basename, rather than full path.
        meta.name = img_basename;
        const fs::path opath{output};
        const fs::path abs_opath = fs::absolute(opath);
        const auto dir = abs_opath.parent_path();
        if (!fs::exists(dir))
            fs::create_directory(dir);

        radial_intensity::SaveRadialIntensityProfile(average_intensities, opath.string(), meta);
    } catch(po_help_exception & e){
        return 1;
    };

}
