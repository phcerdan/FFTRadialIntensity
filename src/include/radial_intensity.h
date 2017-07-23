#ifndef RADIAL_INTENSITY_H
#define RADIAL_INTENSITY_H
#include <vector>
#include <numeric>
#include <algorithm>
#include <fstream>
// #include <boost/filesystem.hpp>
// #include "itkFrequencyShiftedFFTLayoutImageRegionConstIteratorWithIndex.h"

namespace radial_intensity
{

using IntensitiesAtRadius = std::vector<double>;
using Intensities =  std::vector<IntensitiesAtRadius>;
using FlattenIntensities =  std::vector<double>;

struct HistoBins
{
    double left;
    double right;
    double bin_width;
};

/**
 * Return pixel values of image grouped by radial frequencies. We try to bin pixels into radial frequencies boxes.
 * Given an input_img of float pixel types, returns a vector with intensities at different radius.
 *
 * Intensities[0] contains all the pixels in the outer(larger) radial layer.
 * Corner regions are ignored.
 * Corner regions are delimited by the circle/sphere. point (x,y,...) is in corner if : x^2 + y^2 + ... > Modulo(fMax)
 * FNyquist == fMax is image is even.
 * Where fMax_x = fs_x / 2, ; fs_x = 1/dx
 *
 * Note: it is important to distinguist between pixels and physical points.
 * If dealing with pixels/voxels we might need to impose a square or cubic image (same size in every dimension).
 * If dealing with frequencies, we have to choose a deltaF from an fft, or physical points of frequency image.
 */
template<typename ImageType, typename FrequencyIteratorType>
std::pair<Intensities, HistoBins> ComputeRadialFrequencyIntensities(const ImageType * input_img)
{
    using namespace std::literals::string_literals;
    FrequencyIteratorType freqIt(input_img, input_img->GetLargestPossibleRegion());
    /* Set FreqSpacing for the freq range to be:
     * (-0.5,0.5] in all dims,
     * independtly of size, or spatial resolution of image.
     */
    auto freqSpacing = freqIt.GetFrequencySpacing();
    auto inputSize = input_img->GetLargestPossibleRegion().GetSize();
    for (unsigned int dim = 0; dim < ImageType::ImageDimension; ++dim)
    {
        freqSpacing[dim] = 1.0 / inputSize[dim];
    }
    freqIt.SetFrequencySpacing(freqSpacing);

    auto region_size = input_img->GetLargestPossibleRegion().GetSize();
    int Smax = 0;
    for (unsigned int i = 0; i< ImageType::ImageDimension; ++i)
    {
        Smax = std::max(static_cast<int>(region_size[i]), Smax);
    }
    Intensities intensities;
    intensities.resize(Smax + 1);

    // Because freqIt->GetFrequencySpacing() == 1.0, we know that the max frequency allowed is 0.5.
    double w2_max = 0.25; // R^2 = 0.5 * 0.5
    double dbin = w2_max / Smax;
    double w2(0.0);
    int w2bin(0);
    for ( freqIt.GoToBegin(); !freqIt.IsAtEnd(); ++freqIt ) {
        w2 = freqIt.GetFrequencyModuloSquare();
        // w2bin range:= [0,Smax], ...(High Freq corner region)]
        w2bin = static_cast<int>( w2 / dbin) + 1; // (w2/w2max) * Smax

        // if invalid corner region. equivalent to ( w2 > w2_max)
        if ( w2bin > Smax )
            continue;
        else if (w2bin < 0 )
            throw std::logic_error("error, w2bin: "s + std::to_string(w2bin) + " cannot be less than 0"s);
        if (w2 == 0.0)
            w2bin = 0;

        intensities[w2bin].emplace_back(freqIt.Get());
    }

    double left = 0.0;
    double right = 0.5;
    double bin_width = 0.5 / Smax;
    HistoBins histo_bins{ left, right, bin_width };
    return std::make_pair(intensities, histo_bins);
}

FlattenIntensities AverageRadialFrequencyIntensities(const Intensities & intensities)
{
    int d(0);
    double mean(0.0);
    FlattenIntensities intensities_mean(intensities.size());
    for( auto & iarray : intensities){
        mean = std::accumulate(std::begin(iarray), std::end(iarray), 0.0);
        mean = mean / static_cast<double>(iarray.size());
        intensities_mean[d] = mean;
        d++;
    }
    return intensities_mean;
}

bool fileExists(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

struct MetadataFields {
    std::string name;
    std::vector<int> img_size;
};

// Precondition: assume file can be created: \sa fileExists
void SaveRadialIntensityProfile(const FlattenIntensities & mean_intensities, const std::string &fname, const MetadataFields &metadata_fields)
{
    std::ofstream output_file (fname); // delete everything inside the file(default)

    if (!output_file.is_open()) {
        perror( ("Error creating IvsQ file in " + fname).c_str());
    }
    // boost::filesystem::path inpath{inputName_};
    // auto inFileName = inpath.filename().string();
    output_file << "# input_file=" << metadata_fields.name << std::endl;
    output_file << "# Nx=" << metadata_fields.img_size[0] << std::endl;
    output_file << "# Ny=" << metadata_fields.img_size[1] << std::endl;
    if(metadata_fields.img_size.size() == 3)
        output_file << "# Nz=" << metadata_fields.img_size[2] << std::endl;

    for (unsigned int i = 0; i < mean_intensities.size(); i++ ){
        output_file << i  << " " << mean_intensities[i] << std::endl;
    }

    if(output_file.bad()){
        perror( ("Error saving IvsQ file in " + fname ).c_str());
    }
    output_file.close();
}

} // end namespace
#endif
