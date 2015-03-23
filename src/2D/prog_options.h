#include <iostream>
#include <string>
#include <fstream>
#include <exception>
using namespace std;
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
        ("input_img,i", po::value<string>()->required(),
              "input image.")
        ("output,o", po::value<string>()->default_value(""),
              "output result to file.")
        ("load_dist,l", po::value<string>()->default_value(""),
              "load distances_indexes file.")
        ("save_dist,s", po::value<string>()->default_value(""),
              "save distances_indexes to file.")
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
        cout << visible << "\n";
        throw po_help_exception("help exit");
    }

    notify(vm);

    if (vm["output"].as<string>()!="") {
        cout << "Output to : " + vm["output"].as<string>()<< "\n";
    } else {
        cout << "Output to default directory" << "\n";
    }

    if (vm["load_dist"].as<string>()!="") {
        cout << "PixelCenterDistances will be loaded from: " + vm["load_dist"].as<string>() << "\n";
    } else {
        cout << "No PixelCenterDistances will be loaded" << "\n";
    }

    if (vm["save_dist"].as<string>()!="") {
        cout << "PixelCenterDistances will be saved to: " + vm["save_dist"].as<string>() << "\n";
    } else {
        cout << "PixelCenterDistances information will be saved to default directory" << "\n";
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
