#ifndef IMAGE_FUNCTIONS_H
#define IMAGE_FUNCTIONS_H

#include "is_complex_t.h"

#include <itkImage.h>
#include <string>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkForwardFFTImageFilter.h>
#include <itkFFTShiftImageFilter.h>
#include <itkComplexToModulusImageFilter.h>
#include <itkPowImageFilter.h>

namespace imaging {

template <typename ImageType>
typename ImageType::Pointer read_image( const std::string & filename)
{
    using ReaderType = itk::ImageFileReader<ImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    try {
        reader->Update();
    } catch( itk::ExceptionObject & excp ) {
        std::cerr << "Problem encountered while reading image file : " <<
            filename << std::endl;
        throw;
    }
    return reader->GetOutput();
}

template <typename ImageType>
void write_image( const ImageType * img, const std::string & filename)
{
    using WriterType = itk::ImageFileWriter<ImageType>;
    auto writer = WriterType::New();
    writer->SetInput(img);
    writer->SetFileName(filename);

    try {
        writer->Update();
    } catch( itk::ExceptionObject & e ) {
        std::cerr << "Error writing image into file:"
                  << filename << std::endl;
        throw;
    }
}

template <typename ImageType>
typename itk::ForwardFFTImageFilter<ImageType>::OutputImageType::Pointer fft_forward( const ImageType * input_img )
{
    static_assert(!std::is_integral<typename ImageType::PixelType>::value, "fft_forward requires PixelType different than int/char");
    using FFTForwardType = itk::ForwardFFTImageFilter<ImageType>;
    auto fftForward = FFTForwardType::New();
    fftForward->SetInput(input_img);
    fftForward->Update();
    return fftForward->GetOutput();
}

template <typename ImageType>
typename ImageType::Pointer shift_fft( const ImageType * input_img)
{
    // If the image is odd, the inverse transform of a shifted image
    // requires SetInverse(True) on the inverseFFT filter.
    using ShiftType = itk::FFTShiftImageFilter<ImageType, ImageType>;
    auto shift = ShiftType::New();
    shift->SetInput(input_img);
    shift->Update();
    return shift->GetOutput();
}

template <typename ImageType>
typename itk::ForwardFFTImageFilter<ImageType>::OutputImageType::Pointer fft_and_shift( const ImageType * input_img )
{
    return shift_fft(fft_forward(input_img).GetPointer());
}

template <typename ImageType>
typename ImageType::Pointer pow_image(const ImageType * input_img, const double exponent )
{
    static_assert(!std::is_integral<typename ImageType::PixelType>::value, "pow_image requires PixelType different than int/char");

    typedef itk::PowImageFilter<ImageType> PowType;
    auto powFilter     = PowType::New();
    powFilter->SetInput1(input_img);
    powFilter->SetConstant2(exponent);
    powFilter->Update();
    return powFilter->GetOutput();
}

template <typename ComplexImageType>
typename itk::Image<typename ComplexImageType::PixelType::value_type, ComplexImageType::ImageDimension>::Pointer
modulus_fft( const ComplexImageType * fft_img )
{
    static_assert(is_complex_t<typename ComplexImageType::PixelType>::value, "modulus_fft only works on complex pixel types. Apply fft first.");
    using RealPixelType = typename ComplexImageType::PixelType::value_type;
    using RealImageType = itk::Image<RealPixelType, ComplexImageType::ImageDimension>;
    using ComplexToModulusType = itk::ComplexToModulusImageFilter<ComplexImageType, RealImageType>;
    auto modulusFilter = ComplexToModulusType::New();
    modulusFilter->SetInput(fft_img);
    modulusFilter->Update();
    return modulusFilter->GetOutput();
}

template <typename ComplexImageType>
auto modulus_square_fft( const ComplexImageType * fft_img )
{
    // using RealPixelType = typename ComplexImageType::PixelType::value_type;
    // using RealImageType = itk::Image<RealPixelType, ComplexImageType::ImageDimension>;
    auto mod_fft = modulus_fft(fft_img);
    return pow_image(mod_fft.GetPointer(), 2);
}

} // end namespace
#endif
