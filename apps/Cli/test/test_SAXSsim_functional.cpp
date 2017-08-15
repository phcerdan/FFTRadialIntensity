#include "catch.hpp"
#include "radial_intensity.h"
#include "image_functions.h"
#include "saxs_sim_functional.h"
#include "itkFrequencyFFTLayoutImageRegionConstIteratorWithIndex.h"
#include "itkFrequencyShiftedFFTLayoutImageRegionConstIteratorWithIndex.h"
#include <string>
#include <boost/filesystem.hpp>
#include <tuple> // std::tie

namespace fs = boost::filesystem;
const fs::path basepath = "/home/phc/repository_local/FFT-from-image-compute-radial-intensity/build";
const fs::path testpath = basepath / "test_results";

TEST_CASE("imaging functions", "[imaging]"){
    const std::string img_basename{"5x5"};
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
            const std::string out_filename = img_basename + "_fft.mha";
            const fs::path path_out = testpath / out_filename;
            imaging::write_image(fft.GetPointer(), path_out.string());
            SECTION("ComplexModulus"){
                auto modulus_fft = imaging::modulus_fft(fft.GetPointer());
                const std::string out_filename = img_basename + "_mod_fft.mha";
                const fs::path path_out = testpath / out_filename;
                imaging::write_image<ImageType>(modulus_fft, path_out.string());
            }
            SECTION("ComplexModulus Square"){
                auto modulus_square_fft = imaging::modulus_square_fft(fft.GetPointer());
                const std::string out_filename = img_basename + "_mod_square_fft.mha";
                const fs::path path_out = testpath / out_filename;
                imaging::write_image<ImageType>(modulus_square_fft, path_out.string());
            }
        }
        SECTION("fft_forward_shifted"){
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
template<typename ImageType, typename FrequencyImageIterator>
radial_intensity::Intensities TestHelperComputeRadialFrequencyIntensities(const std::string & img_path_string)
{
    auto img = imaging::read_image<ImageType>(img_path_string);
    auto pair_intensities_histobins = radial_intensity::ComputeRadialFrequencyIntensities<ImageType, FrequencyImageIterator>(img);
    auto & intensities = std::get<0>(pair_intensities_histobins);
    auto & histo_bins = std::get<1>(pair_intensities_histobins);
    radial_intensity::PrintIntensitiesHisto(intensities, histo_bins);
    return intensities;
}

TEST_CASE("ComputeRadialFrequencyIntensities odd", "[radial_intensity][odd]"){
    const std::string img_basename{"5x5"};
    using ImageType = itk::Image<float, 2>;

    // FFT standard layout
    const std::string img_filename_fft{img_basename + "_mod_square_fft.mha"};
    const fs::path path_img_fft = basepath / "fixtures" / img_filename_fft;
    using FrequencyImageIteratorFFT = itk::FrequencyFFTLayoutImageRegionConstIteratorWithIndex<ImageType>;
    auto intensities_fft = TestHelperComputeRadialFrequencyIntensities<ImageType, FrequencyImageIteratorFFT>(path_img_fft.string());

    // FFT shifted
    const std::string img_filename_fft_shifted{img_basename + "_mod_square_fft_shifted.mha"};
    const fs::path path_img_fft_shifted = basepath / "fixtures" / img_filename_fft_shifted;
    using FrequencyImageIteratorShiftedFFT = itk::FrequencyShiftedFFTLayoutImageRegionConstIteratorWithIndex<ImageType>;
    auto intensities_fft_shifted = TestHelperComputeRadialFrequencyIntensities<ImageType, FrequencyImageIteratorShiftedFFT>(path_img_fft_shifted.string());

    //Sort && Compare
    for( auto & intensity_at_radius : intensities_fft )
        std::sort(std::begin(intensity_at_radius), std::end(intensity_at_radius));
    for( auto & intensity_at_radius : intensities_fft_shifted )
        std::sort(std::begin(intensity_at_radius), std::end(intensity_at_radius));

    auto bins = intensities_fft.size();
    CHECK(bins == intensities_fft_shifted.size());
    for (size_t b = 0; b < bins; ++b)
    {
        const auto & fft_bin_intensities = intensities_fft[b];
        const auto & fft_shifted_bin_intensities = intensities_fft_shifted[b];
        CHECK(fft_bin_intensities == fft_shifted_bin_intensities);
    }
}

TEST_CASE("ComputeRadialFrequencyIntensities even", "[radial_intensity][even]"){
    const std::string img_basename{"disc20"};
    using ImageType = itk::Image<float, 2>;

    // FFT standard layout
    const std::string img_filename_fft{img_basename + "_mod_square_fft.mha"};
    const fs::path path_img_fft = basepath / "fixtures" / img_filename_fft;
    using FrequencyImageIteratorFFT = itk::FrequencyFFTLayoutImageRegionConstIteratorWithIndex<ImageType>;
    auto intensities_fft = TestHelperComputeRadialFrequencyIntensities<ImageType, FrequencyImageIteratorFFT>(path_img_fft.string());

    // FFT shifted
    const std::string img_filename_fft_shifted{img_basename + "_mod_square_fft_shifted.mha"};
    const fs::path path_img_fft_shifted = basepath / "fixtures" / img_filename_fft_shifted;
    using FrequencyImageIteratorShiftedFFT = itk::FrequencyShiftedFFTLayoutImageRegionConstIteratorWithIndex<ImageType>;
    auto intensities_fft_shifted = TestHelperComputeRadialFrequencyIntensities<ImageType, FrequencyImageIteratorShiftedFFT>(path_img_fft_shifted.string());

    //Sort && Compare
    for( auto & intensity_at_radius : intensities_fft )
        std::sort(std::begin(intensity_at_radius), std::end(intensity_at_radius));
    for( auto & intensity_at_radius : intensities_fft_shifted )
        std::sort(std::begin(intensity_at_radius), std::end(intensity_at_radius));

    auto bins = intensities_fft.size();
    CHECK(bins == intensities_fft_shifted.size());
    for (size_t b = 0; b < bins; ++b)
    {
        const auto & fft_bin_intensities = intensities_fft[b];
        const auto & fft_shifted_bin_intensities = intensities_fft_shifted[b];
        CHECK(fft_bin_intensities == fft_shifted_bin_intensities);
    }
}

TEST_CASE("AverageIntensities", "[average]"){
    radial_intensity::IntensitiesAtRadius i0{5000};
    radial_intensity::IntensitiesAtRadius i1{1000, 2000, 3000};
    radial_intensity::IntensitiesAtRadius i2{20, 10, 30, 50, 40};
    radial_intensity::Intensities intensities{i0,i1,i2};
    SECTION("Average Intensities"){
        auto average_intensities = radial_intensity::AverageRadialFrequencyIntensities(intensities);
        REQUIRE(average_intensities.size() == 3 );
        CHECK(average_intensities[0] == Approx(intensities[0][0]));
        CHECK(average_intensities[1] == Approx(2000));
        CHECK(average_intensities[2] == Approx(30));

        SECTION("SaveRadialIntensityProfile"){
            const std::string img_basename = "fake";
            const std::string plot_filename{img_basename + "_plot.txt"};
            const fs::path plot_path = testpath / plot_filename;
            radial_intensity::MetadataFields meta;
            meta.name = img_basename;
            meta.size = {5,5};
            radial_intensity::SaveRadialIntensityProfile(average_intensities, plot_path.string(), meta);
        }
    }
}
