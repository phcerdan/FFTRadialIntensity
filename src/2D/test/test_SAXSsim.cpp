#include "gmock/gmock.h"
#include "SAXSsim.h"
#include <memory>
using namespace testing;
using namespace std;
//static parameters.
struct SAXSsimTest : public ::testing::Test{
    static const string img;
    static shared_ptr<SAXSsim> sim;

    static void SetUpTestCase(){
        sim = make_shared<SAXSsim>(img) ;
    };
};
const string SAXSsimTest::img{"./fixtures/imgTiny.tiff"};
shared_ptr<SAXSsim> SAXSsimTest::sim;// = make_shared<SAXSsim>(imgTiny);
TEST_F(SAXSsimTest,ReadImages){
    sim->Read(img);
}
TEST_F(SAXSsimTest,DFT){
    auto I = sim->Read(img);
    sim->DFT(I);
    // sim->Show();
}
TEST_F(SAXSsimTest, CalculateIntensities) {

    auto I = sim->Read(img);
    auto D = sim->DFT(I);
    sim->PixelDistances(D);
    sim->IntensityFromDistanceVector();
    auto iv = sim->MeanIntensities();
    for( auto &i : iv){
        cout << i << " ";
    }
    cout << endl;
    sim->Show();
}

struct img5x5_F : public ::testing::Test{
    static const string img;
    static shared_ptr<SAXSsim> sim;

    static void SetUpTestCase(){
        sim = make_shared<SAXSsim>(img) ;
    };
};
const string img5x5_F::img{"./fixtures/5x5.tiff"};
shared_ptr<SAXSsim> img5x5_F::sim;

TEST_F(img5x5_F, PixelDistances){
    EXPECT_EQ(5, sim->I_.cols);
    EXPECT_EQ(5, sim->I_.rows);
    EXPECT_EQ( 5, sim->dft_size.first );
    EXPECT_EQ( 5, sim->dft_size.second );
    EXPECT_FALSE(sim->even_flag.first);
    EXPECT_FALSE(sim->even_flag.second);
    unsigned int total_indexes{0};
    for (auto &v : sim->distances_indexes.ind){
        total_indexes += v.size();
    }
    EXPECT_EQ(25, total_indexes);
}

struct img4x4_F : public ::testing::Test{
    static const string img;
    static shared_ptr<SAXSsim> sim;

    static void SetUpTestCase(){
        sim = make_shared<SAXSsim>(img) ;
    };
};
const string img4x4_F::img{"./fixtures/4x4.tiff"};
shared_ptr<SAXSsim> img4x4_F::sim;

TEST_F(img4x4_F, PixelDistances){
    EXPECT_EQ(4, sim-> I_.cols);
    EXPECT_EQ(4, sim-> I_.rows);
    EXPECT_EQ( 4, sim->dft_size.first );
    EXPECT_EQ( 4, sim->dft_size.second );
    EXPECT_TRUE(sim->even_flag.first);
    EXPECT_TRUE(sim->even_flag.second);

    SAXSsim::index_pair_vector d0{};
    EXPECT_EQ(d0, sim->distances_indexes.ind[0]);

    SAXSsim::index_pair_vector d1{SAXSsim::index_pair{2,2}, SAXSsim::index_pair{2,1},
        SAXSsim::index_pair{1,2}, SAXSsim::index_pair{1,1}};
    EXPECT_EQ(d1, sim->distances_indexes.ind[1]);

    SAXSsim::index_pair_vector d2{SAXSsim::index_pair{2,3}, SAXSsim::index_pair{2,0},
        SAXSsim::index_pair{1,3}, SAXSsim::index_pair{1,0}, SAXSsim::index_pair{3,2},
        SAXSsim::index_pair{3,1}, SAXSsim::index_pair{0,2}, SAXSsim::index_pair{0,1},
        SAXSsim::index_pair{3,3}, SAXSsim::index_pair{3,0},
        SAXSsim::index_pair{0,3}, SAXSsim::index_pair{0,0}};
    EXPECT_EQ(d2, sim->distances_indexes.ind[2]);

    // // Corner indexes are also in d3. /TODO
    // SAXSsim::index_pair_vector d3{SAXSsim::index_pair{3,3}, SAXSsim::index_pair{3,0},
    //     SAXSsim::index_pair{0,3}, SAXSsim::index_pair{0,0}};
    // EXPECT_EQ(d3, s.distances_indexes.ind[3]);
    unsigned int total_indexes{0};
    for (auto &v : sim->distances_indexes.ind){
        total_indexes += v.size();
    }
    EXPECT_EQ(16, total_indexes);
    unsigned int total_intensities{0};
    for (auto &v : sim->intensities_at_distance){
        total_intensities += v.size();
    }
    EXPECT_EQ(total_indexes, total_intensities);
}

struct img4x5_F : public ::testing::Test{
    static const string img;
    static shared_ptr<SAXSsim> sim;

    static void SetUpTestCase(){
        sim = make_shared<SAXSsim>(img) ;
    };
};
const string img4x5_F::img{"./fixtures/4x5.tiff"};
shared_ptr<SAXSsim> img4x5_F::sim;

TEST_F(img4x5_F, PixelDistances){
    EXPECT_EQ(4, sim->I_.cols);
    EXPECT_EQ(5, sim->I_.rows);
    EXPECT_EQ( 4, sim->dft_size.first );
    EXPECT_EQ( 5, sim->dft_size.second );
    EXPECT_TRUE(sim->even_flag.first);
    EXPECT_FALSE(sim->even_flag.second);
    unsigned int total_indexes{0};
    for (auto &v : sim->distances_indexes.ind){
        total_indexes += v.size();
    }
    EXPECT_EQ(20, total_indexes);
}
