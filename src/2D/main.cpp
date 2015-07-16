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
#include "SAXSsim.h"
using namespace std;
int main(int argc, char* argv[]){

    try {
        auto option_map = program_options(argc, argv);
        string input = option_map["input_img"].as<string>();
        string output = option_map["output"].as<string>();

        #ifdef ENABLE_PARALLEL
        int num_threads = option_map["num_threads"].as<int>();
        auto sim = SAXSsim(input, output, num_threads);
        #else
        auto sim = SAXSsim(input, output);
        #endif
    } catch(po_help_exception & e){
        return 1;
    };

}
