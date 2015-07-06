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
#include <QuickView.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdexcept>
#include <iostream>
#include <fstream>
// #include <cereal/archives/portable_binary.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
using serialize_input_type = boost::archive::text_iarchive;
using serialize_output_type = boost::archive::text_oarchive;
// using serialize_input_type = cereal::PortableBinaryInputArchive;
// using serialize_output_type = cereal::PortableBinaryOutputArchive;
// #include <cereal/archives/json.hpp>
// using cereal_input_type = cereal::JSONInputArchive;
// using cereal_output_type = cereal::JSONOutputArchive;
using namespace cv;
using namespace itk;
using namespace std;
SAXSsim::SAXSsim(const string inputName, string outputName, int numThreads, bool saveToFile) :
    inputName_{inputName}, numThreads_{numThreads}
{
    input.img = inputName;
    // Measure execution time
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    Read(inputName_);
    FFT();
    FFTModulusSquare(fftImg_);

    // Compute Intensities
    cout << "Computing Intensity..." << endl;
#ifdef ENABLE_PARALLEL
    // int max_threads = omp_get_max_threads();
    int max_threads = omp_get_num_procs();
    if (numThreads_ > max_threads) numThreads_ = max_threads;
    omp_set_num_threads(numThreads_);
    cout << "Number of Threads:" << numThreads_ <<" MaxThreads: " << max_threads << endl;
    input.threads = numThreads_;
    if(numThreads_>1) ParallelComputeRadialIntensity();
    else ComputeRadialIntensity();
#else
    ComputeRadialIntensity();
#endif
    MeanIntensities();

    // Save results. Get the filename of input with no extension
    if (saveToFile){
        boost::filesystem::path opath{inputName};
        auto path_no_extension    = opath.stem();
        auto input_no_extension = path_no_extension.generic_string();
        if (outputName == "")
            outputName = "./results/" + input_no_extension + ".plot";
        input.outPlot = outputName;
        SaveIntensityProfile(outputName);
    }
    // Display execution time
    end = chrono::system_clock::now();
    chrono::duration<double, ratio<60>> elapsed_time = end-start;
    cout << "Elapsed time: " << elapsed_time.count() << " min" << endl;
}
SAXSsim::~SAXSsim(){}

SAXSsim::InputTypeP  SAXSsim::Read(const string &inputName){
    // Reader
    typedef itk::ImageFileReader< InputImageType > ReaderType;
    auto reader = ReaderType::New();

    // SCIFIO type
    // typedef itk::SCIFIOImageIO  ImageIOType;
    // TIFF type
    typedef itk::TIFFImageIO ImageIOType;

    auto scifioIO = ImageIOType::New();
    reader->SetImageIO(scifioIO);
    reader->SetFileName( inputName );

    try {
        reader->Update();
    } catch( itk::ExceptionObject & excp ) {
        std::cerr << "Problem encountered while reading image file : " <<
            inputName << std::endl;
        throw;
    }

    // typedef itk::CastImageFilter< InputImageType, InputImageType > castFilterType;
    // castFilterType::Pointer castFilter = castFilterType::New();
    // castFilter->SetInput(reader->GetOutput());
    // castFilter->Update();

    inputImg_ = reader->GetOutput();

    // QuickView viewer;
    // viewer.AddImage(inputImg_);
    // viewer.Visualize();
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
void SAXSsim::WriteFFTModulus( const string &outputFilename){

    typedef itk::ComplexToModulusImageFilter<ComplexImageType, RealImageType> FftModulusType;
    auto modulusFilter = FftModulusType::New();
    modulusFilter->SetInput(fftImg_);
    typedef unsigned short                           OutputPixelType;
    typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
    typedef itk::RescaleIntensityImageFilter<RealImageType, OutputImageType > RescaleFilter;
    auto rescaleFilter = RescaleFilter::New();
    rescaleFilter->SetInput(modulusFilter->GetOutput());
    rescaleFilter->SetOutputMinimum( itk::NumericTraits< OutputPixelType >::min() );
    rescaleFilter->SetOutputMaximum( itk::NumericTraits< OutputPixelType >::max() );
    typedef  itk::ImageFileWriter< OutputImageType  > WriterType;
    auto writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(rescaleFilter->GetOutput());
    writer->Update();
}
void SAXSsim::GeneratePDF(const string & resultfile, double image_resolution){
    // Execute R script to generate pdf with the plot.
    string command{"./plotI-q.R " + resultfile + " "+ std::to_string(image_resolution)};
    cout << "executing command: " << command << endl;
    system(command.c_str());

    // Open generated pdf.
    int lastindex = resultfile.find_last_of(".");
    string filename_no_ext = resultfile.substr(0, lastindex);
    string pdf{"evince " + filename_no_ext + ".pdf"};
    cout << "opening pdf: " << pdf << endl;
    system(pdf.c_str());
}
SAXSsim::RealTypeP & SAXSsim::FFTModulusSquare(const SAXSsim::ComplexTypeP & D){
    typedef itk::ComplexToModulusImageFilter<ComplexImageType, RealImageType> FftModulusType;
    auto modulusFilter = FftModulusType::New();
    modulusFilter->SetInput(D);
    modulusFilter->Update();

    typedef itk::PowImageFilter<RealImageType, RealImageType, RealImageType> PowType;
    auto powFilter     = PowType::New();
    powFilter->SetInput1(modulusFilter->GetOutput());
    powFilter->SetConstant2(2);
    powFilter->Update();
    return fftModulusSquare_ = powFilter->GetOutput();
}
SAXSsim::RealTypeP SAXSsim::LogFFTModulusSquare(const SAXSsim::RealTypeP & modulo){
    typedef itk::LogImageFilter<RealImageType, RealImageType> LogFilterType;
    auto logFilter = LogFilterType::New();
    logFilter->SetInput(modulo);
    logFilter->Update();
    return logFilter->GetOutput();
}

SAXSsim::ComplexTypeP & SAXSsim::FFT(){
    // Tranform the input image to real value to perform FFT.
    typedef itk::CastImageFilter< InputImageType, RealImageType > FilterType;
    auto filter = FilterType::New();
    filter->SetInput(inputImg_);
    filter->Update();
    RealImageType* realInputImg = filter->GetOutput();

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
    intensities_.resize(fMax_ + 1);

    double I{0}, d_aprox{0};
    int  d{0} ;
    // RealImageType::SizeType size;
    // size[0] = imgSize_.first;
    // size[1] = midSize_.second;
    // cout << size[0] << " sizes " << size[1] << endl;
    // auto region = fftModulusSquare_->GetRequestedRegion();
    RealImageType::IndexType pixelIndex;
    RealImageType::IndexValueType pixelValue;

    for( int y = 0; y < midSize_.second +1; y++){
        for( int x = 0; x < imgSize_.first; x++){
            d_aprox = Modulo<double>(abs(x - midSize_.first) ,abs( y - midSize_.second));
            d = static_cast<int>(d_aprox);
            if (d > fMax_) continue;
            pixelIndex[0] = x;
            pixelIndex[1] = y;
            pixelValue = fftModulusSquare_->GetPixel(pixelIndex);
            I = pixelValue;
            intensities_[d].push_back(I);
        }
    }
    return intensities_;
}

#ifdef ENABLE_PARALLEL
SAXSsim::Intensities & SAXSsim::ParallelComputeRadialIntensity(){
    cout << "Calculating intensities in parallel" << endl;
    intensities_.resize(fMax_ + 1);

    // RealImageType::SizeType size;
    // size[0] = imgSize_.first;
    // size[1] = midSize_.second;
    // fftModulusSquare_->SetRegions(size);

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
            RealImageType::IndexValueType pixelValue;
            pixelIndex[0] = x;
            pixelIndex[1] = y;
            pixelValue = fftModulusSquare_->GetPixel(pixelIndex);
            I = pixelValue;
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
