#include <iostream>
#include "prog_options.h"
#include "SAXSsim.h"
using namespace std;
int main(int argc, char* argv[]){

    auto option_map = program_options(argc, argv);
    string input = option_map["input_file"].as<string>();
    auto sim = SAXSsim(input);
    //
}
//implement 2d using openCV gpu.
//opencv has no 3d capabilities. ITK does, but do not have a clear(to me) gpu support.
