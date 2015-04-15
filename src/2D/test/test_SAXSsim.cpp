#include "gmock/gmock.h"
#include "SAXSsim.h"
#include <memory>
using namespace testing;
using namespace std;

TEST(img5x5, CorrectNumberOfIntensities){
    const string img{"./fixtures/5x5.tiff"};
    auto sim = make_shared<SAXSsim>(img) ;
    EXPECT_EQ( 5, sim->imgSize_.first );
    EXPECT_EQ( 5, sim->imgSize_.second );
    EXPECT_FALSE(sim->evenFlag_.first);
    EXPECT_FALSE(sim->evenFlag_.second);
    int total_indices{0};
    for (auto &i : sim->intensities_){
        total_indices += i.size();
    }
    EXPECT_EQ(15, total_indices);
}

TEST(img4x4_F, CorrectNumberOfIntensities){
    const string img{"./fixtures/4x4.tiff"};
    auto sim = make_shared<SAXSsim>(img) ;
    EXPECT_EQ( 4, sim->imgSize_.first );
    EXPECT_EQ( 4, sim->imgSize_.second );
    EXPECT_TRUE(sim->evenFlag_.first);
    EXPECT_TRUE(sim->evenFlag_.second);

    int total_indices{0};
    for (auto &i : sim->intensities_){
        total_indices += i.size();
    }
    EXPECT_EQ(12, total_indices);
}


TEST(disc20_F, writeDFT){
    const string img{"./fixtures/disc20.tif"};
    auto sim = make_shared<SAXSsim>(img) ;
    string output_f = "./results/discFFT.png";
    sim->WriteFFTModulus( output_f);
    auto sim20x5 = make_shared<SAXSsim>("./fixtures/disc20x5.tif");
    output_f = "./results/disc20x5FFT.png";
    sim20x5->WriteFFTModulus( output_f);
}

#ifdef ENABLE_PARALLEL
struct parallel_img5x5_F : public ::testing::Test{
    static const string img;
    static shared_ptr<SAXSsim> sim;

    static void SetUpTestCase(){
        sim = make_shared<SAXSsim>(img, "", 4) ;
    };
};
const string parallel_img5x5_F::img{"./fixtures/5x5.tiff"};
shared_ptr<SAXSsim> parallel_img5x5_F::sim;

TEST_F(parallel_img5x5_F, IntensityComparisson_single){
    shared_ptr<SAXSsim> sim_single = make_shared<SAXSsim>(img);
    auto i_p                 =  sim->intensities_;
    auto i_s                 =  sim_single->intensities_;
    double sump = 0;
    double sums = 0;
    for(auto id : i_p){
        for(auto iv : id){
            sump += iv;
        }
    }
    for(auto id : i_s){
        for(auto iv : id){
            sums += iv;
        }
    }
    EXPECT_EQ(sump, sums);
}

TEST_F(parallel_img5x5_F, IntensityComparissonDifferent_j){
    shared_ptr<SAXSsim> sim_maxj = make_shared<SAXSsim>(img, "./results/5x5p_jmax.plot", omp_get_num_procs());
    auto i_p                 =  sim->MeanIntensities();
    auto i_s                 =  sim_maxj->MeanIntensities();
    EXPECT_EQ(i_p, i_s);
}

#endif // ENABLE_PARALLEL
