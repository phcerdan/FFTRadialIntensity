#include "gmock/gmock.h"
#include "SAXSsim.h"
#include <memory>
#include "prog_options_singleImagesFromMontage.h"
using namespace testing;
using namespace std;

string root;
string tail;
string output;
int initNumber;
int finalNumber;
string separator;
int num_threads = 1;
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

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
        cout << "USAGE: -i /path/to/rootOfStack -t .tif -o results/output.plot -j12"<< endl;
        cout << "Default parsing of the montage is: i_j:"<< endl;
        return 1;
    };

    return RUN_ALL_TESTS();
}

TEST(pectin1, Original){
    string inputImg;
    string outputF;
    std::vector<double> meanIntensity;
    for (int i = initNumber; i<=finalNumber; ++i ){
        for (int j = initNumber; j <= finalNumber; ++j) {
            inputImg = root + to_string(i) + separator + to_string(j) + tail;
            outputF = output + to_string(i) + separator + to_string(j) ;
            shared_ptr<SAXSsim> sim = make_shared<SAXSsim>(inputImg, outputF, num_threads);
            if(i == initNumber && j == initNumber){
                meanIntensity = sim->MeanIntensities();
            } else {
                //Sum
                std::size_t idx = 0;
                for (auto v : sim->MeanIntensities()){
                    meanIntensity[idx] += v;
                    idx++;
                }
                //Divide by the totalNumber of images
                if(i == finalNumber && j == finalNumber){
                    for (auto& t : meanIntensity) {
                        auto totalNumber =  pow(1 + finalNumber - initNumber, 2);
                        t /= totalNumber;
                    }
                }
            }
        }
    }

    outputF                 = output + "MEAN";
    shared_ptr<SAXSsim> sim = make_shared<SAXSsim>(inputImg, outputF, num_threads, 0);
    sim->intensitiesMean_    = meanIntensity;
    sim->SaveIntensityProfile(outputF);

}


