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
#include "SAXSsim.h"
#include <memory>
#include "prog_options_singleImagesFromMontage.h"
using namespace std;

int main(int argc, char** argv) {

    string root;
    string tail;
    string output;
    int initNumber;
    int finalNumber;
    string separator;
    int num_threads = 1;

    // Parse command line
    try {
        auto option_map = program_options_singleImagesFromMontage(argc, argv);
        root = option_map["root"].as<string>();
        tail       = option_map["tail"].as<string>();
        initNumber = option_map["initNumber"].as<int>();
        finalNumber = option_map["finalNumber"].as<int>();
        separator = option_map["separator"].as<string>();
        output = option_map["output"].as<string>();
#ifdef ENABLE_PARALLEL
        num_threads = option_map["num_threads"].as<int>();
#endif
    } catch(po_help_exception & e){
        cout << "USAGE: -r /path/to/rootOfStack -i 0 -f 4 -separator _ -t .tif -o results/outputroot -j12"<< endl;
        cout << "Default parsing of the montage is: i<separator>j:"<< endl;
        return 1;
    };

    string inputImg;
    string outputF;
    std::vector<double> meanIntensity;
    for (int i = initNumber; i<=finalNumber; ++i ){
        for (int j = initNumber; j <= finalNumber; ++j) {
            // Call SAXSsim for every image and save output:
            inputImg = root + to_string(i) + separator + to_string(j) + tail;
            outputF = output + to_string(i) + separator + to_string(j) + ".plot" ;
            shared_ptr<SAXSsim> sim = make_shared<SAXSsim>(inputImg, outputF, num_threads);
        }
    }

    return 1;
}



