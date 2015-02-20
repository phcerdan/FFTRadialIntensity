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
        ("config,c", po::value<string>()->default_value("config_file.init"),
              "name of a file of a configuration.")
        ;

    // Declare a group of options that will be
    // allowed both on command line and in
    // config file
    po::options_description config("Configuration");
    config.add_options()
        ("value", po::value<int>()->default_value(20),
              "value description")
        ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file", po::value< vector<string> >(), "input file")
        ;

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    po::options_description visible("Allowed options");
    visible.add(generic).add(config);

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    store(po::command_line_parser(argc, argv).
          options(cmdline_options).positional(p).run(), vm);
    notify(vm);
    if (vm.count("help")) {
        cout << visible << "\n";
        return 0;
    }

    string config_file = vm["config"].as<string>();
    ifstream ifs(config_file.c_str());
    if (!ifs)
    {
        throw(runtime_error("Can not open config file: " + config_file + " ,use -c option"));
    }
    else
    {
        store(parse_config_file(ifs, config_file_options), vm);
        notify(vm);
    }
    return vm;
}
