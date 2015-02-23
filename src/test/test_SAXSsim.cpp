#include "gmock/gmock.h"
#include "SAXSsim.h"
#include <memory>
using namespace testing;
using namespace std;
//static parameters.
struct SAXSsimTest : public ::testing::Test{
    // Input Parameters to SAXSsim constructor.
    static string imgTiny;
    // Instance of SAXSsim class that is shared for the testCase.
    static shared_ptr<SAXSsim> sim;
    static void SetUpTestCase(){
        sim = make_shared<SAXSsim>(imgTiny) ;
    };
};
string SAXSsimTest::imgTiny{"./fixtures/imgTiny.tiff"};
shared_ptr<SAXSsim> SAXSsimTest::sim;// = make_shared<SAXSsim>(imgTiny);

TEST_F(SAXSsimTest,ReadImages){
    sim->Read(imgTiny);
}
TEST_F(SAXSsimTest,DFT){
    auto I = sim->Read(imgTiny);
    sim->DFT(I);
    // sim->Show();
}
TEST_F(SAXSsimTest, Scatter){
    auto I = sim->Read(imgTiny);
    auto D = sim->DFT(I);
    sim->Scatter(D);
}
