#ifndef FUNCTIONAL_SAXS_SIM_H
#define FUNCTIONAL_SAXS_SIM_H
#include "radial_intensity.h"
#include "image_functions.h"
namespace radial_intensity
{

template<typename ImageType, typename FrequencyIteratorType>
std::pair<Intensities, HistoBins> ComputeRadialFrequencyIntensityFromImage(const std::string & filename)
{
    using T = ImageType;
    using namespace imaging;
    auto input_image = read_image<T>(filename);
    auto fft = fft_forward<T>(input_image);
    // using ComplexT = typename std::decay<decltype(*fft)>::type;
    auto fft_modulo_square = modulus_square_fft(fft.GetPointer());
    // Compute intensities
    return ComputeRadialFrequencyIntensities<T, FrequencyIteratorType>(fft_modulo_square);
    // auto intensities = ComputeRadialFrequencyIntensities<T, FrequencyIteratorType>(fft_modulo_square);
    // return AverageRadialFrequencyIntensities<T>(intensities);
}
}
#endif
