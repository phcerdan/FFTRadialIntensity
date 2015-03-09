#include <iostream>
#include "prog_options.h"
#include "SAXSsim.h"
using namespace std;
int main(int argc, char* argv[]){

    try {
        auto option_map = program_options(argc, argv);
        string input = option_map["input_img"].as<string>();
        string output = option_map["output"].as<string>();
        string save_dist = option_map["save_dist"].as<string>();
        string load_dist = option_map["load_dist"].as<string>();
        auto sim = SAXSsim(input, output, save_dist, load_dist);

    } catch(po_help_exception & e){
        return 1;
    };

    //
}
//implement 2d using openCV gpu.
//opencv has no 3d capabilities. ITK does, but do not have a clear(to me) gpu support.
