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
#include "SAXSsim.h"
#include <itkForwardFFTImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkSCIFIOImageIO.h>
#include <itkTIFFImageIO.h>
#include <itkFFTShiftImageFilter.h>
#include <itkComplexToModulusImageFilter.h>
#include <itkPowImageFilter.h>
#include <itkLogImageFilter.h>
#include <itkCastImageFilter.h>
#include "itkIntensityWindowingImageFilter.h"
#include <QuickView.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
using namespace itk;
using namespace std;
SAXSsim::SAXSsim(string inputName, string outputName, int numThreads, bool saveToFile,
        bool delayedInitialize) :
    inputName_{inputName}, outputName_{outputName},
    numThreads_{numThreads}, saveToFile_{saveToFile}
#ifdef ENABLE_QT
    , m_messenger{new QtMessenger}
#endif
{
}

SAXSsim::SAXSsim(string inputName, string outputName, int numThreads, bool saveToFile):
    SAXSsim::SAXSsim(inputName, outputName, numThreads, saveToFile, false)
{

    Initialize();
}
void SAXSsim::SetInputParameters(string inputName, string outputName, int numThreads, bool saveToFile)
{

    inputName_  = inputName ;
    outputName_ = outputName ;
    numThreads_ = numThreads ;
    saveToFile_ = saveToFile ;

}
void SAXSsim::Initialize()
{
    // Measure execution time
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    std::string msg;
    msg = "Reading Image: " + inputName_;
    std::cout << msg << std::endl;
#ifdef ENABLE_QT
    QString Qmsg;
    Qmsg = QString::fromStdString(msg);
    m_messenger->message(Qmsg);
#endif

    Read();
    FFT();
    FFTModulusSquare();

    // Compute Intensities
    msg = "Computing Intensity...";
    cout << msg << endl;
#ifdef ENABLE_QT
    Qmsg = QString::fromStdString(msg);
    m_messenger->message(Qmsg);
#endif
#ifdef ENABLE_PARALLEL
    // int max_threads = omp_get_max_threads();
    int max_threads = omp_get_num_procs();
    if (numThreads_ > max_threads) numThreads_ = max_threads;
    omp_set_num_threads(numThreads_);
    msg = "Number of Threads: " + std::to_string(numThreads_)+ " MaxThreads: " + std::to_string(max_threads);
    cout << msg << endl;
#ifdef ENABLE_QT
    Qmsg = QString::fromStdString(msg);
    m_messenger->message(Qmsg);
#endif
    if(numThreads_>1) ParallelComputeRadialIntensity();
    else ComputeRadialIntensity();
#else
    ComputeRadialIntensity();
#endif
    MeanIntensities();

    // Display execution time
    end = chrono::system_clock::now();
    chrono::duration<double, ratio<60>> elapsed_time = end-start;
    msg = "Elapsed time: " +  std::to_string(elapsed_time.count()) + " min";
    cout << msg << endl;
#ifdef ENABLE_QT
    Qmsg = QString::fromStdString(msg);
    m_messenger->message(Qmsg);
#endif
    // Save results. Get the filename of input with no extension
    if (saveToFile_){
        boost::filesystem::path opath{inputName_};
        auto path_no_extension    = opath.stem();
        auto input_no_extension = path_no_extension.generic_string();
        if (outputName_ == "")
            outputName_ = "./results/" + input_no_extension + ".plot";
        msg = "Output: I vs q. Saving as: " + outputName_;
        cout << msg << endl;
#ifdef ENABLE_QT
        Qmsg = QString::fromStdString(msg);
        m_messenger->message(Qmsg);
#endif
        SaveIntensityProfile(outputName_);
    }
}
SAXSsim::~SAXSsim(){}
SAXSsim::InputTypeP  SAXSsim::Read(){
    typedef itk::ImageFileReader< InputImageType > ReaderType;
    auto reader = ReaderType::New();

    reader->SetFileName( inputName_ );

    try {
        reader->Update();
    } catch( itk::ExceptionObject & excp ) {
        std::cerr << "Problem encountered while reading image file : " <<
            inputName_ << std::endl;
        throw;
    }

    inputImg_ = reader->GetOutput();

    CheckEqualDimension();
    InitializeSizeMembers();
    return inputImg_;
}

void SAXSsim::CheckEqualDimension(){
    auto region = inputImg_->GetLargestPossibleRegion();
    auto x      = region.GetSize()[0];
    auto y      = region.GetSize()[1];
    if(x != y) throw runtime_error("Input Image must have equal dimension in x and y");
}

void SAXSsim::SaveIntensityProfile(const string & fname){

    boost::filesystem::path opath{fname};
    boost::filesystem::create_directories(opath.parent_path());
    std::ofstream output_file (fname); // delete everything inside the file(default)

    if (!output_file.is_open()) {
        perror( ("Error creating IvsQ file in " + opath.string() ).c_str());
    }
    boost::filesystem::path inpath{inputName_};
    auto inFileName = inpath.filename().string();
    output_file << "# input_file=" << inFileName << std::endl;
    output_file << "# Nx=" << imgSize_.first << std::endl;
    output_file << "# Ny=" << imgSize_.second << std::endl;

    for (unsigned int i = 0; i < intensitiesMean_.size(); i++ ){
        output_file << i  << " " << this->intensitiesMean_[i] << std::endl;
    }

    if(output_file.bad()){
        perror( ("Error saving IvsQ file in " + opath.string() ).c_str());
    }
    output_file.close();
}
SAXSsim::RealTypeP SAXSsim::WindowingFFT(
            const SAXSsim::RealTypeP& modulusFFT,
            SAXSsim::RealPixelType maxInputValue,
            SAXSsim::RealPixelType maxOutputValue /**=255 */)
{

    typedef itk::IntensityWindowingImageFilter<RealImageType, RealImageType > WindowingFilter;
    auto windowFilter = WindowingFilter::New();
    windowFilter->SetInput(modulusFFT);
    windowFilter->SetWindowMinimum( 0 );
    windowFilter->SetWindowMaximum( maxInputValue );
    windowFilter->SetOutputMinimum( 0 );
    windowFilter->SetOutputMaximum( maxOutputValue );

    windowFilter->Update();
    return windowFilter->GetOutput();
}
SAXSsim::OutputTypeP SAXSsim::RescaleFFT(const SAXSsim::RealTypeP& inputFFT)
{
    typedef itk::RescaleIntensityImageFilter<RealImageType, OutputImageType > RescaleFilter;
    auto rescaleFilter = RescaleFilter::New();
    rescaleFilter->SetInput(inputFFT);
    rescaleFilter->SetOutputMinimum( itk::NumericTraits< OutputPixelType >::min() );
    rescaleFilter->SetOutputMaximum( itk::NumericTraits< OutputPixelType >::max() );
    rescaleFilter->Update();
    return rescaleFilter->GetOutput();

}

void SAXSsim::WriteFFT( const RealTypeP & fftInput, const string &outputFilename )
{
    // Cast to float (.tif)
    typedef float FloatPixelType;
    typedef itk::Image<OutputPixelType, 2> FloatImageType;
    typedef itk::RescaleIntensityImageFilter<RealImageType, FloatImageType > RescaleFilter;
    auto rescaleFilter = RescaleFilter::New();
    rescaleFilter->SetInput(fftInput);
    rescaleFilter->SetOutputMinimum( 0 );
    rescaleFilter->SetOutputMaximum( itk::NumericTraits< FloatPixelType >::max() );
    rescaleFilter->Update();

    typedef  itk::ImageFileWriter< FloatImageType  > WriterType;
    auto writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(rescaleFilter->GetOutput());
    writer->Update();
}

// Square-root and log of input vector, save to member.
void SAXSsim::ScaleForVisualization()
{
    intensitiesVisualization_.resize(intensitiesMean_.size());
    std::transform(intensitiesMean_.begin(), intensitiesMean_.end(),intensitiesVisualization_.begin(), (double(*)(double))log );
    // std::transform(intensitiesMean_.begin(), intensitiesMean_.end(),intensitiesVisualization_.begin(), (double(*)(double))sqrt );
    // std::transform(intensitiesVisualization_.begin(), intensitiesVisualization_.end(),intensitiesVisualization_.begin(), (double(*)(double)) log );

    LogFFTModulusSquare(fftModulusSquare_);
}
std::string SAXSsim::GeneratePDF(
        const string & resultInputFile,
        double nm_per_pixel_resolution,
        const string & scriptFile){
    // Execute R script to generate pdf with the plot.
    string command{scriptFile + " " + resultInputFile + " " + std::to_string(nm_per_pixel_resolution)};
    cout << "executing command: " << command << endl;
    system(command.c_str());

    // // Open generated pdf.
    // int lastindex = resultInputFile.find_last_of(".");
    // string filename_no_ext = resultInputFile.substr(0, lastindex);
    // string pdf{"evince " + filename_no_ext + ".pdf"};
    // cout << "opening pdf: " << pdf << endl;
    // system(pdf.c_str());

    std::size_t found = resultInputFile.find_last_of("/\\");
    std::string path = resultInputFile.substr(0,found);
    std::string file = resultInputFile.substr(found+1);
    int lastindex = file.find_last_of(".");
    string filename_no_ext = file.substr(0, lastindex);
    return  path + "/" + "pdf" + "/" +  filename_no_ext + ".svg";
}

std::string SAXSsim::GenerateSVG(
        const string & resultInputFile,
        double nm_per_pixel_resolution,
        const string & scriptFile){
    // Execute R script to generate pdf with the plot.
    string command{scriptFile + " " + resultInputFile + " " + std::to_string(nm_per_pixel_resolution)};
    cout << "executing command: " << command << endl;
    system(command.c_str());

    // int lastindex = resultInputFile.find_last_of(".");
    // string filename_no_ext = resultInputFile.substr(0, lastindex);
    // The output after the RScript set in the default script.
    std::size_t found = resultInputFile.find_last_of("/\\");
    std::string path = resultInputFile.substr(0,found);
    std::string file = resultInputFile.substr(found+1);
    int lastindex = file.find_last_of(".");
    string filename_no_ext = file.substr(0, lastindex);
    return  path + "/" + "svg" + "/" +  filename_no_ext + ".svg";

}
#include <itkImageRegionConstIteratorWithIndex.h>
SAXSsim::RealTypeP & SAXSsim::FFTModulusSquare(){

    typedef itk::ComplexToModulusImageFilter<ComplexImageType, RealImageType> FftModulusType;
    auto modulusFilter = FftModulusType::New();
    modulusFilter->SetInput(fftImg_);
    modulusFilter->Update();

    typedef itk::PowImageFilter<RealImageType, RealImageType, RealImageType> PowType;
    auto powFilter     = PowType::New();
    powFilter->SetInput1(modulusFilter->GetOutput());
    powFilter->SetConstant2(2);
    powFilter->Update();
    return fftModulusSquare_ = powFilter->GetOutput();
}
SAXSsim::RealTypeP& SAXSsim::LogFFTModulusSquare(const SAXSsim::RealTypeP & modulo){
    typedef itk::LogImageFilter<RealImageType, RealImageType> LogFilterType;
    auto logFilter = LogFilterType::New();
    logFilter->SetInput(modulo);
    logFilter->Update();
    return fftVisualization_ = logFilter->GetOutput();
}

SAXSsim::ComplexTypeP & SAXSsim::FFT(){
    // Tranform the input image to real value to perform FFT.
    typedef itk::CastImageFilter< InputImageType, RealImageType > FilterType;
    auto filter = FilterType::New();
    filter->SetInput(inputImg_);
    filter->Update();
    auto realInputImg = filter->GetOutput();

    // FFT
    typedef itk::ForwardFFTImageFilter<RealImageType, ComplexImageType> fFFTType;
    fFFTType::Pointer fft = fFFTType::New();
    fft->SetInput(realInputImg);
    fft->Update();
    fftImg_ = fft->GetOutput();

    // Shift/Center FFT.
    // If the image is odd, the inverse transform of a shifted image
    // requires SetInverse(True) on the inverseFFT filter.
    typedef itk::FFTShiftImageFilter<ComplexImageType, ComplexImageType> ShiftType;
    auto shift = ShiftType::New();
    shift->SetInput(fftImg_);
    shift->Update();
    return fftImg_ = shift->GetOutput();
}

void SAXSsim::InitializeSizeMembers(){
    auto region = inputImg_->GetLargestPossibleRegion();
    auto size = region.GetSize();
    midSize_  = make_pair (size[0]/2, size[1]/2);
    imgSize_  = make_pair (size[0], size[1]);
    // Nyquist freq in both directions.
    // fmax = make_pair (imgSize_.first/2.0,imgSize_.second/2.0);
    // dfx = dfy in images.
    fMax_ = min(midSize_.first, midSize_.second) ;
    evenFlag_ = make_pair (size[0] % 2 == 0, size[1] % 2 == 0);

}


std::vector<double> & SAXSsim::MeanIntensities(){
    int d{0};
    double mean{0};
    intensitiesMean_.clear();
    intensitiesMean_.resize(intensities_.size());
    for( auto & dv : intensities_){
        if (dv.size() != 0) {
            mean = 0.0;
            for (auto & iv : dv ){
               mean+=iv;
            }
            mean = mean / static_cast<double> (dv.size());
            intensitiesMean_[d] = mean;
        }
        d++;
    }
    return intensitiesMean_;
}

SAXSsim::Intensities & SAXSsim::ComputeRadialIntensity(){
    intensities_.clear();
    intensities_.resize(fMax_ + 1);

    double I{0}, d_aprox{0};
    int  d{0} ;
    RealImageType::IndexType pixelIndex;

    for( int y = 0; y < midSize_.second +1; y++){
        for( int x = 0; x < imgSize_.first; x++){
            d_aprox = Modulo<double>(abs(x - midSize_.first) ,abs( y - midSize_.second));
            d = static_cast<int>(d_aprox);
            if (d > fMax_) continue;
            pixelIndex[0] = x;
            pixelIndex[1] = y;
            I = fftModulusSquare_->GetPixel(pixelIndex);
            intensities_[d].push_back(I);
        }
    }
    return intensities_;
}

#ifdef ENABLE_PARALLEL
SAXSsim::Intensities & SAXSsim::ParallelComputeRadialIntensity(){
    cout << "Calculating intensities in parallel" << endl;
    intensities_.clear();
    intensities_.resize(fMax_ + 1);

    // create intensities_ for each thread.
    vector<Intensities> p_intensities;
    int Nthreads  = numThreads_;
    p_intensities.resize(Nthreads);
    for (int N    = 0;  N < Nthreads; N++ ){
        p_intensities[N].assign(intensities_.begin(), intensities_.end());
    }
    // int debug_counts{0};

    for( int x = 0; x < imgSize_.first; x++){
        #pragma omp parallel for
        for( int y = 0; y < midSize_.second +1; y++){
            int th = omp_get_thread_num();
            double I{0}, d_aprox{0};
            int  d{0} ;
            d_aprox = Modulo<double>(abs(x - midSize_.first), abs(y - midSize_.second));
            d = static_cast<int>(d_aprox);
            if (d > fMax_) continue;

            RealImageType::IndexType pixelIndex;
            pixelIndex[0] = x;
            pixelIndex[1] = y;
            I = fftModulusSquare_->GetPixel(pixelIndex);
            p_intensities[th][d].push_back(I);
        }
    }
    // MERGE p_intensities into intensities_
    for (int N    = 0;  N < numThreads_; N++ ){
        for (int _d = 0 ; _d < fMax_ + 1; _d++){
            auto it_end     = intensities_[_d].end();
            auto it_p_begin = p_intensities[N][_d].begin();
            auto it_p_end = p_intensities[N][_d].end();
            intensities_[_d].insert(it_end, it_p_begin, it_p_end);
        }
    }
    return intensities_;
}
#endif
