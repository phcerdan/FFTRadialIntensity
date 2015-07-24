/**
 FFT From Image. Apply to a microscopy image, returning a I-q data set,
 allowing comparisson with Small Angle X-ray Scattering experiments.
 Copyright © 2015 Pablo Hernandez-Cerdan

 This library is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, see <http://www.gnu.org/licenses/>.
*/
#include <memory>
#include "catch.hpp"
#include "SAXSsim.h"
using namespace std;

TEST_CASE("Intensities per pixel in ODD image", "[img5x5]"){
    const string img{"./fixtures/5x5.tiff"};
    auto sim = make_shared<SAXSsim>(img) ;
    REQUIRE( sim->imgSize_.first == 5 );
    REQUIRE( sim->imgSize_.second == 5);
    REQUIRE( sim->evenFlag_.first == false);
    REQUIRE( sim->evenFlag_.second == false);
    int total_indices{0};
    for (auto &i : sim->intensities_){
        total_indices += i.size();
    }
    REQUIRE(sim->fMax_ + 1 == sim->intensities_.size());
    REQUIRE(total_indices == 15);
}
TEST_CASE("Intensities per pixel in EVEN image", "[img4x4]"){

    const string img{"./fixtures/4x4.tiff"};
    auto sim = make_shared<SAXSsim>(img) ;
    REQUIRE( sim->imgSize_.first == 4 );
    REQUIRE( sim->imgSize_.second == 4 );
    REQUIRE( sim->evenFlag_.first == true);
    REQUIRE(sim->evenFlag_.second == true);

    int total_indices{0};
    for (auto &i : sim->intensities_){
        total_indices += i.size();
    }
    REQUIRE(total_indices == 12);
}

TEST_CASE("Write FFT file", "[disc20]" ){
    const string img{"./fixtures/disc20.tif"};
    auto sim = make_shared<SAXSsim>(img) ;
    string output_f = "./results/discFFT.tif";
    sim->ScaleForVisualization();
    sim->WriteFFT(
            sim->WindowingFFT(
                sim->fftVisualization_, sim->intensitiesVisualization_[1], 255),
            output_f);
    auto sim20x5 = make_shared<SAXSsim>("./fixtures/disc20x5.tif");
    output_f = "./results/disc20x5FFT.tif";
    sim->ScaleForVisualization();
    sim->WriteFFT(
            sim->WindowingFFT(
                sim->fftVisualization_, sim->intensitiesVisualization_[1], 255),
            output_f);
    // sim20x5->WriteFFT( sim->fftModulusSquare_, output_f);
}

#ifdef ENABLE_PARALLEL

TEST_CASE("Setup a 2 core test SAXSsim", "[parallel]"){
    const string img{"./fixtures/5x5.tiff"};
    shared_ptr<SAXSsim> simParallel = make_shared<SAXSsim>(img, "", 2);

    SECTION(" Compare with Single thread"){
        shared_ptr<SAXSsim> simSingle = make_shared<SAXSsim>(img);
        auto i_p                 =  simParallel->intensities_;
        auto i_s                 =  simSingle->intensities_;
        REQUIRE(i_p.size() == i_s.size());
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
        REQUIRE(sump == Approx(sums));
    }

    SECTION(" Compare with Max Cores"){
        shared_ptr<SAXSsim> simMaxCores = make_shared<SAXSsim>(img, "./results/5x5p_jmax.plot", omp_get_num_procs());
        auto i_p                 =  simParallel->MeanIntensities();
        auto i_s                 =  simMaxCores->MeanIntensities();
        REQUIRE(i_p.size() == i_s.size());
        for (size_t ind = 0; ind < i_p.size(); ind++ ){
            REQUIRE(i_p[ind] == Approx(i_s[ind]));
        }
        // REQUIRE(i_p == i_s);
    }
}

#endif // ENABLE_PARALLEL
