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
#include <string>
#include <fstream>
#include <exception>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
struct po_help_exception: public std::runtime_error {
    po_help_exception(const std::string & s) : std::runtime_error(s){};
};

po::variables_map program_options(const int &argc, char** const & argv)
{
    // Declare a group of options that will be
    // allowed only on command line
    po::options_description generic("Generic options");
    generic.add_options()
        ("version,v", "print version string")
        ("help,h", "produce help message")
        ("input_img,i", po::value<std::string>()->required(),
              "input image.")
        ("output,o", po::value<std::string>()->default_value(""),
              "output result to file.")
        ;
#ifdef ENABLE_PARALLEL
    generic.add_options()
        ("num_threads,j", po::value<int>()->default_value(1),
              "number of threads using omp.")
        ;
#endif
    po::options_description cmdline_options;
    cmdline_options.add(generic);

    po::options_description visible("Allowed options");
    visible.add(generic);

    po::variables_map vm;
    store(po::command_line_parser(argc, argv).
          options(cmdline_options).run(), vm);
    if (vm.count("help") || !vm.count("input_img")) {
        std::cout << visible << "\n";
        throw po_help_exception("help exit");
    }

    notify(vm);

    if (vm["output"].as<std::string>()!="") {
        std::cout << "Output to : " + vm["output"].as<std::string>()<< "\n";
    } else {
        std::cout << "Output to default directory" << "\n";
    }

    return vm;
}
