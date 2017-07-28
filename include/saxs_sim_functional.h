#ifndef FUNCTIONAL_SAXS_SIM_H
#define FUNCTIONAL_SAXS_SIM_H
#include "radial_intensity.h"
#include "image_functions.h"
#include "itkFrequencyFFTLayoutImageRegionConstIteratorWithIndex.h"
namespace radial_intensity
{

using IntensitiesHistoMetaTuple = std::tuple<Intensities, HistoBins, MetadataFields >;

template<typename ImageType>
IntensitiesHistoMetaTuple ComputeRadialIntensitiyFromImage(
        const std::string & filename)
{
    using T = ImageType;
    auto input_image = imaging::read_image<T>(filename);
    auto fft = imaging::fft_forward<T>(input_image);
    auto fft_modulo_square = imaging::modulus_square_fft(fft.GetPointer());
    // Compute intensities using fft (not shifted)
    using FrequencyImageIterator = itk::FrequencyFFTLayoutImageRegionConstIteratorWithIndex<ImageType>;
    const auto intensities_histo = ComputeRadialFrequencyIntensities<T, FrequencyImageIterator>(fft_modulo_square);
    const auto & intensities = std::get<0>(intensities_histo);
    const auto & histo = std::get<1>(intensities_histo);

    radial_intensity::MetadataFields meta;
    meta.name = filename;
    auto itk_size = input_image->GetLargestPossibleRegion().GetSize();
    for (unsigned int i = 0; i < T::ImageDimension; ++i)
    {
        meta.size.push_back(itk_size[i]);
    }
    return std::make_tuple(intensities, histo, meta);
}
} //end namespace
#endif
