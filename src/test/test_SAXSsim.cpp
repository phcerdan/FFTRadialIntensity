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
static string img5x5{"./fixtures/5x5.tiff"};
static string img4x4{"./fixtures/4x4.tiff"};
static string img4x5{"./fixtures/4x5.tiff"};
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
    auto histo = sim->Scatter(D);
    histo.Save("histo_test");
}
TEST_F(SAXSsimTest, CalculateIntensities) {

    auto I = sim->Read(imgTiny);
    auto D = sim->DFT(I);
    sim->PixelDistances(D);
    cout << "PixelDistancesFinished" <<endl;
    // auto inten = sim->IntensityFromDistanceVector(D);
}
TEST(PixelDistances, img5x5){
    auto s = SAXSsim (img5x5);
    auto I = s.Read(img5x5);
    EXPECT_EQ(5, I.cols);
    EXPECT_EQ(5, I.rows);

    auto D = s.DFT(I);
    s.PixelDistances(D);
    EXPECT_EQ( 5, s.dft_size.first );
    EXPECT_EQ( 5, s.dft_size.second );
    EXPECT_FALSE(s.even_flag.first);
    EXPECT_FALSE(s.even_flag.second);
    unsigned int total_indexes{0};
    for (auto &v : s.distances_indexes){
        cout << v.size() <<endl;
        total_indexes += v.size();
        for (auto &p : v){
            cout << p[0] << " " << p[1] <<endl;
        }
    }
    EXPECT_EQ(25, total_indexes);
}
TEST(PixelDistances, img4x4){
    auto s = SAXSsim(img4x4);
    auto I = s.Read(img4x4);
    EXPECT_EQ(4, I.cols);
    EXPECT_EQ(4, I.rows);

    auto D = s.DFT(I);
    s.PixelDistances(D);
    EXPECT_EQ( 4, s.dft_size.first );
    EXPECT_EQ( 4, s.dft_size.second );
    EXPECT_TRUE(s.even_flag.first);
    EXPECT_TRUE(s.even_flag.second);
    SAXSsim::index_pair_vector d0{};
    EXPECT_EQ(d0, s.distances_indexes[0]);
    SAXSsim::index_pair_vector d1{SAXSsim::index_pair{2,2}, SAXSsim::index_pair{2,1},
        SAXSsim::index_pair{1,2}, SAXSsim::index_pair{1,1}};
    EXPECT_EQ(d1, s.distances_indexes[1]);
    SAXSsim::index_pair_vector d2{SAXSsim::index_pair{2,3}, SAXSsim::index_pair{2,0},
        SAXSsim::index_pair{1,3}, SAXSsim::index_pair{1,0}, SAXSsim::index_pair{3,2},
        SAXSsim::index_pair{3,1}, SAXSsim::index_pair{0,2}, SAXSsim::index_pair{0,1},
        SAXSsim::index_pair{3,3}, SAXSsim::index_pair{3,0},
        SAXSsim::index_pair{0,3}, SAXSsim::index_pair{0,0}};
    EXPECT_EQ(d2, s.distances_indexes[2]);
    // // Corner indexes are also in d3. /TODO
    // SAXSsim::index_pair_vector d3{SAXSsim::index_pair{3,3}, SAXSsim::index_pair{3,0},
    //     SAXSsim::index_pair{0,3}, SAXSsim::index_pair{0,0}};
    // EXPECT_EQ(d3, s.distances_indexes[3]);
    unsigned int total_indexes{0};
    for (auto &v : s.distances_indexes){
        cout << v.size() <<endl;
        total_indexes += v.size();
        for (auto &p : v){
            cout << p[0] << " " << p[1] <<endl;
        }
    }
    EXPECT_EQ(16, total_indexes);
}

TEST(PixelDistances, img4x5){
    auto s = SAXSsim(img4x5);
    auto I = s.Read(img4x5);
    EXPECT_EQ(4, I.cols);
    EXPECT_EQ(5, I.rows);

    auto D = s.DFT(I);
    s.PixelDistances(D);
    EXPECT_EQ( 4, s.dft_size.first );
    EXPECT_EQ( 5, s.dft_size.second );
    EXPECT_TRUE(s.even_flag.first);
    EXPECT_FALSE(s.even_flag.second);
    unsigned int total_indexes{0};
    for (auto &v : s.distances_indexes){
        // cout << v.size() <<endl;
        total_indexes += v.size();
        // for (auto &p : v){
        //     cout << p[0] << " " << p[1] <<endl;
        // }
    }
    EXPECT_EQ(20, total_indexes);
}
