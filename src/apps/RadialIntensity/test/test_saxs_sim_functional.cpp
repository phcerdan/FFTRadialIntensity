#include "catch.hpp"
#include "radial_intensity.h"
#include "image_functions.h"
#include "saxs_sim_functional.h"
#include "itkFrequencyFFTLayoutImageRegionConstIteratorWithIndex.h"
#include <string>
#include <boost/filesystem.hpp>
#include <tuple> // std::tie

namespace fs = boost::filesystem;
const fs::path basepath = "/home/phc/repository_local/FFT-from-image-compute-radial-intensity/build";
const fs::path testpath = basepath / "test_results";
const std::string img_basename{"5x5"};

TEST_CASE("imaging functions", "[imaging]"){
const std::string img_filename{img_basename + ".tiff"};
    const fs::path path_img = basepath / "fixtures" / img_filename;
    using ImageType = itk::Image<float, 2>;
    SECTION("read_image"){
        auto img = imaging::read_image<ImageType>(path_img.string());
        SECTION("write_image"){
            fs::create_directories(testpath);
            const std::string out_filename = img_basename + ".mha";
            const fs::path path_out = testpath / out_filename;
            imaging::write_image<ImageType>(img, path_out.string());
        }
        SECTION("Pow"){
            auto pow_image = imaging::pow_image<ImageType>(img, 2);
        }
        SECTION("fft_forward"){
            auto fft = imaging::fft_forward<ImageType>(img);
            auto fft_shifted = imaging::fft_and_shift<ImageType>(img);
            const std::string out_filename = img_basename + "_fft_shifted.mha";
            const fs::path path_out = testpath / out_filename;
            imaging::write_image(fft_shifted.GetPointer(), path_out.string());
            SECTION("ComplexModulus"){
                auto modulus_fft = imaging::modulus_fft(fft_shifted.GetPointer());
                const std::string out_filename = img_basename + "_mod_fft_shifted.mha";
                const fs::path path_out = testpath / out_filename;
                imaging::write_image<ImageType>(modulus_fft, path_out.string());
            }
            SECTION("ComplexModulus Square"){
                auto modulus_square_fft = imaging::modulus_square_fft(fft_shifted.GetPointer());
                const std::string out_filename = img_basename + "_mod_square_fft_shifted.mha";
                const fs::path path_out = testpath / out_filename;
                imaging::write_image<ImageType>(modulus_square_fft, path_out.string());
            }
        }
    }
}

TEST_CASE("radial_intensity functions", "[radial_intensity]"){
    const std::string img_filename{img_basename + "_mod_square_fft_shifted.mha"};
    fs::path path_img = basepath / "fixtures" / img_filename;
    using ImageType = itk::Image<float, 2>;
    auto img = imaging::read_image<ImageType>(path_img.string());
    SECTION("ComputeRadialFrequencyIntensities"){
        using FrequencyImageIterator = itk::FrequencyFFTLayoutImageRegionConstIteratorWithIndex<ImageType>;
        auto pair_intensities_histobins = radial_intensity::ComputeRadialFrequencyIntensities<ImageType, FrequencyImageIterator>(img);
        auto & intensities = std::get<0>(pair_intensities_histobins);
        auto & histo_bins = std::get<1>(pair_intensities_histobins);
        int d = 0;
        std::cout << "Histogram: start-end: [ " << histo_bins.left << "," << histo_bins.right << " ] ; bin_width: " << histo_bins.bin_width << std::endl;
        for (auto &radius_index : intensities){
            double left = (d==0)? 0 : (d - 1) * histo_bins.bin_width;
            double right = (d==0)? 0 : (d) * histo_bins.bin_width;
            std::cout << "Index: " << d << " ; Bin: [ " << left << ", " <<right << " ]" << std::endl;
            std::cout << " #pixels at this radius: " << radius_index.size() << std::endl;
            std::cout << " Intensities: [ ";
            for (auto &i : radius_index){
                std::cout << i << ", ";
            }
            std::cout << " ]" << std::endl;
            ++d;
        }
    }
}

TEST_CASE("ComputeRadialFrequencyIntensityFromImage: ODD", "[img5x5]"){
    const std::string img_filename{"/home/phc/repository_local/FFT-from-image-compute-radial-intensity/src/fixtures/5x5.tiff"};
    using ImageType= itk::Image<float, 2>;
    using FrequencyImageIterator = itk::FrequencyFFTLayoutImageRegionConstIteratorWithIndex<ImageType>;
    auto pair_intensities_histobins = radial_intensity::ComputeRadialFrequencyIntensityFromImage<ImageType, FrequencyImageIterator>(img_filename);
    auto & intensities = std::get<0>(pair_intensities_histobins);
    int total_indices{0};
    for (auto &i : intensities){
        total_indices += i.size();
    }
    auto ibins = intensities.size();
    REQUIRE(ibins == 6 );
    CHECK(total_indices == 25);
    SECTION("Average Intensities"){
        auto average_intensities = radial_intensity::AverageRadialFrequencyIntensities(intensities);
        REQUIRE(average_intensities.size() == ibins );
        CHECK(average_intensities[ibins-2] == Approx(252));
    }
}
