#ifndef RADIAL_INTENSITY_H
#define RADIAL_INTENSITY_H
#include <vector>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iostream>

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
using IntensitiesHistoPair = std::pair<Intensities, HistoBins>;

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
IntensitiesHistoPair ComputeRadialFrequencyIntensities(const ImageType * input_img)
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
    // bins is the number of bins to calculate intensities.
    // because symmetry, is half the image size.
    // the min from the size dimensions is taken.
    int bins = std::numeric_limits<int>::max() ;
    for (unsigned int i = 0; i< ImageType::ImageDimension; ++i)
    {
        bins = std::min(static_cast<int>(region_size[i] / 2), bins);
        // bins = s/2
    }
    Intensities intensities;
    // + 1 because 0 index is special.
    intensities.resize(bins + 1);

    // Because freqIt->GetFrequencySpacing() == 1.0, we know that the max frequency allowed is 0.5.
    // 0.5 is Nyquist, which only appears in even images (shared between + and - sides).
    // If image is odd, Nyquist is not present and the max frequency is lower.
    // But it doesn't matter
    double w_max = 0.5;
    double dbin = (w_max + std::numeric_limits<double>::epsilon()) / bins;
    for ( freqIt.GoToBegin(); !freqIt.IsAtEnd(); ++freqIt ) {
        double w = sqrt(freqIt.GetFrequencyModuloSquare());
        // w2in range:= [0,bins], ...(High Freq corner region)]
        double wbin = static_cast<int>( w / dbin) + 1; // (w2/w2max) * bins

        // if invalid corner region. equivalent to ( w > w_max). Nyquist is ignored.
        if ( wbin > bins )
            continue;
        else if (wbin < 0 )
            throw std::logic_error("error, wbin: "s + std::to_string(wbin) + " cannot be less than 0"s);
        if (w == 0.0)
            wbin = 0;

        intensities[wbin].emplace_back(freqIt.Get());
    }

    double left = 0.0;
    double right = 0.5;
    double bin_width = 0.5 / bins;
    HistoBins histo_bins{ left, right, bin_width };
    return std::make_pair(intensities, histo_bins);
}

FlattenIntensities AverageRadialFrequencyIntensities(const Intensities & intensities);
void PrintIntensitiesHisto(const Intensities & intensities, const HistoBins & histo_bins, std::ostream & os = std::cout);
bool fileExists(const char *fileName);
struct MetadataFields {
    std::string name;
    std::vector<int> size;
};

// Precondition: assume file can be created: \sa fileExists
void SaveRadialIntensityProfile(const FlattenIntensities & mean_intensities, const std::string &fname, const MetadataFields &metadata_fields);

} // end namespace
#endif
