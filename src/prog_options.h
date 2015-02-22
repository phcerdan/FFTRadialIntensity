#include <iostream>
#include <string>
#include <fstream>
using namespace std;
#include <boost/program_options.hpp>
namespace po = boost::program_options;

po::variables_map program_options(const int &argc, char** const & argv)
{
    // Declare a group of options that will be
    // allowed only on command line
    po::options_description generic("Generic options");
    generic.add_options()
        ("version,v", "print version string")
        ("help,h", "produce help message")
        ("input_file,i", po::value<string>()->required(),
              "input image.")
        ;

    po::options_description cmdline_options;
    cmdline_options.add(generic);

    po::options_description visible("Allowed options");
    visible.add(generic);

    po::variables_map vm;
    store(po::command_line_parser(argc, argv).
          options(cmdline_options).run(), vm);
    notify(vm);
    if (vm.count("help")) {
        cout << visible << "\n";
        return 0;
    }

    // string config_file = vm["config"].as<string>();
    // ifstream ifs(config_file.c_str());
    // if (!ifs)
    // {
    //     throw(runtime_error("Can not open config file: " + config_file + " ,use -c option"));
    // }
    // else
    // {
    //     store(parse_config_file(ifs, config_file_options), vm);
    //     notify(vm);
    // }
    return vm;
}
