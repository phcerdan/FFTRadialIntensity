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

    //
}
//implement 2d using openCV gpu.
//opencv has no 3d capabilities. ITK does, but do not have a clear(to me) gpu support.
