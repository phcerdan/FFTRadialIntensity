#include "SAXSsim.h"
#include <itkForwardFFTImageFilter.h>
#include <itkFFTShiftImageFilter.h>
#include "itkComplexToModulusImageFilter.h"
#include <itkLogImageFilter.h>
#include <itkCastImageFilter.h>
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
SAXSsim::SAXSsim(const string inputName, string outputName, string save_dist, string load_dist, int num_threads) :
    inputName_{inputName}, num_threads_{num_threads}
{
    input.img = inputName;
    // Measure execution time
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    Read(inputName_);
    DFT();
    string serialization_string = "./serialization/x"+ std::to_string(dft_size.first) + "_y" + std::to_string(dft_size.second);
    if(load_dist =="" && ! boost::filesystem::exists(serialization_string) ){
        cout << "Computing new set of distances_index..." << endl;
        PixelDistances(dftMat_);

    } else {
        if (load_dist == ""){
            cout << "Loading suitable serialization archive with distances index"<<endl;
            load_dist = serialization_string;
            input.loadIndices = load_dist;
        }
        PixelCenterDistances loaded_data;
        {
            ifstream sinp(load_dist);
            serialize_input_type iarchive(sinp);
            iarchive >> loaded_data ;
        }

        if(loaded_data.Nx == dft_size.first && loaded_data.Ny == dft_size.second){

            distance_indices = loaded_data;
            InitializeSizeMembers(dftMat_);

        } else
            throw std::runtime_error("Size of input image:[ " + std::to_string(dft_size.first) + ", "
                    + std::to_string(dft_size.second) +  " ] is different from loaded data: [ "
                    + std::to_string(loaded_data.Nx) + ", " + std::to_string(loaded_data.Ny) + " ]\n"
                    "Change the data to load, resize the image, or generate new distance_indices"
                    "running the executable without -l or --load_dist option. "
             );
    }

    if(save_dist == "")
        save_dist = serialization_string;
        input.saveIndices = save_dist;
    {
        boost::filesystem::path opath{save_dist};
        boost::filesystem::create_directories(opath.parent_path());
        ofstream sout(save_dist);
        serialize_output_type oarchive(sout);
        oarchive << distance_indices;
    }

    cout << "Computing Intensity..." << endl;
#ifdef ENABLE_PARALLEL
    // int max_threads = omp_get_max_threads();
    int max_threads = omp_get_num_procs();
    if (num_threads_ > max_threads) num_threads_ = max_threads;
    omp_set_num_threads(num_threads_);
    cout << "Number of Threads:" << num_threads_ <<" MaxThreads: " << max_threads << endl;
    input.threads = num_threads_;
    if(num_threads_>1) ParallelIntensityFromDistanceVector();
    else IntensityFromDistanceVector();
#else
    IntensityFromDistanceVector();
#endif
    MeanIntensities();
    // Save results. Get the filename of input with no extension
    boost::filesystem::path opath{inputName};
    auto path_no_extension    = opath.stem();
    auto input_no_extension = path_no_extension.generic_string();
    if (outputName == "")
        outputName = "./results/" + input_no_extension + ".plot";
    input.outPlot = outputName;
    SavePlot(outputName);
    // Show();

    // Display execution time
    end = chrono::system_clock::now();
    chrono::duration<double, ratio<60>> elapsed_time = end-start;
    cout << "Elapsed time: " << elapsed_time.count() << " min" << endl;
}
SAXSsim::~SAXSsim(){}

SAXSsim::InputImageType* & SAXSsim::Read(const string &inputName){
    // Reader
    typedef itk::ImageFileReader< InputImageType > ReaderType;
    auto reader = ReaderType::New();

    // IO type
    typedef itk::SCIFIOImageIO  ImageIOType;
    auto scifioIO = ImageIOType::New();

    reader->SetImageIO(scifioIO);
    reader->SetFileName( inputName );

    try {
        reader->Update();
    } catch( itk::ExceptionObject & excp ) {
        std::cerr << "Problem encoutered while reading image file : " <<
            inputName << std::endl;
    }

    I_ = *reader->GetOutput();
    CheckEqualDimension();
    return I_;
}

void SAXSsim::CheckEqualDimension(){
    auto &region = I_->GetLargestPossibleRegion();
    auto x      = region.GetSize()[0];
    auto y      = region.GetSize()[1];
    if(x != y) throw runtime_error("Input Image must have equal dimension in x and y");
}
void SAXSsim::SavePlot(const string & fname){

    boost::filesystem::path opath{fname};
    boost::filesystem::create_directories(opath.parent_path());
    std::ofstream output_file (fname); // delete everything inside the file(default)

    if (!output_file.is_open()) {
        perror( ("Error creating IvsQ file in " + opath.string() ).c_str());
    }
    boost::filesystem::path inpath{inputName_};
    auto inFileName = inpath.filename().string();
    output_file << "# input_file=" << inFileName << std::endl;
    output_file << "# Nx=" << dft_size.first << std::endl;
    output_file << "# Ny=" << dft_size.second << std::endl;

    for (long long i = 0; i!=this->distance_indices.ind.size() - 1; i++ ){
        output_file << i  << " " << this->intensities_mean[i] << std::endl;
    }

    if(output_file.bad()){
        perror( ("Error saving IvsQ file in " + opath.string() ).c_str());
    }
    output_file.close();
}
void SAXSsim::ShowPlot(const string & resultfile, double image_resolution){
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
void SAXSsim::logDFT(Mat &D){
    typedef itk::ComplexToModulusImageFilter<DFTImageType, RealImageType> FftModulusType;
    auto modulusFilter = FftModulusType::New();
    modulusFilter->SetInput(dftMat_);
    modulusFilter->Update();
    fftModulus_ = modulusFilter->GetOutput();

    typedef itk::LogImageFilter<RealImageType, RealImageType> LogFilterType;
    auto logFilter = LogFilterType::New();
    logFilter->SetInput(fftModulus_);
    logFilter->Update();
    fftModulus_ = logFilter->GetOutput();
    D += Scalar::all(1);
    log(D,D);
    log_flag_ = true;
}

SAXSsim::DFTImageType* & SAXSsim::DFT(){
    // Tranform the input image to real value to perform FFT.
    typedef itk::CastImageFilter< InputImageType, RealImageType > FilterType;
    auto filter = FilterType::New();
    filter->SetInput(I_);
    filter->Update();
    RealImageType* realI_ = filter->GetOutput();

    // FFT
    typedef itk::ForwardFFTImageFilter<RealImageType, DFTImageType> fFFTType;
    fFFTType::Pointer fft = fFFTType::New();
    fft->SetInput(realI_);
    fft->Update();
    dftMat_ = fft->GetOutput();

    // Shift/Center FFT.
    // If the image is odd, the inverse transform of a shifted image
    // requires SetInverse(True) on the inverseFFT filter.
    typedef itk::FFTShiftImageFilter<DFTImageType, DFTImageType> ShiftType;
    auto shift = ShiftType::New();
    shift->SetInput(dftMat_);
    shift->Update();
    dftMat_ = shift->GetOutput();

    return dftMat_;
}

void SAXSsim::InitializeSizeMembers(const cv::Mat & dftMat){
    mid_size  = make_pair (dftMat.cols/2, dftMat.rows/2);
    dft_size  = make_pair (dftMat.cols, dftMat.rows);
    // Nyquist freq in both directions.
    // fmax = make_pair (dft_size.first/2.0,dft_size.second/2.0);
    // dfx = dfy in images.

    even_flag = make_pair (dftMat.cols % 2 == 0, dftMat.rows % 2 == 0);
    // double origin_x = even_flag.first ? 0.0 : 0.5;
    // double origin_y = even_flag.second ? 0.0 : 0.5;
    // origin = make_pair(origin_x, origin_y);

    // double distance_max = Modulo<double>(mid_size.first + origin.first, mid_size.second + origin.second);
    d_assigned_max = min(mid_size.first, mid_size.second) ;

    // xi_begin = even_flag.first ? 0 : 1;
    // xi_end = even_flag.first ? mid_size.first : mid_size.first +1;
    // yi_begin = even_flag.second ? 0 : 1;
    // yi_end = even_flag.second ? mid_size.second : mid_size.second +1 ;
}


std::vector<double> & SAXSsim::MeanIntensities(){
    int d{0};
    double mean{0};
    intensities_mean.resize(distance_indices.ind.size());
    for( auto & dv : intensities_at_distance){
        if (dv.size() != 0) {
            mean = 0.0;
            for (auto & iv : dv ){
               mean+=iv;
            }
            mean = mean / static_cast<double> (dv.size());
            intensities_mean[d] = mean;
        }
        d++;
    }
    return intensities_mean;
}

SAXSsim::intensities_vector & SAXSsim::IntensityFromDistanceVector(){
    intensities_at_distance.resize(distance_indices.ind.size());
    int d_max = intensities_at_distance.size() - 1;

    float *p;
    double I{0}, d_aprox{0};
    int  d{0} ;

    auto it_begin = distance_indices.ind[0].begin(),
         it_end = distance_indices.ind[0].end(),
         it_found = it_end;
    // int debug_counts{0};
    index_pair ipair{};
    for( int y = 0; y < distance_indices.Ny/2 +1; y++){
        p = dftMat_.ptr<float>(y);
        for( int x = 0; x < distance_indices.Nx; x++){
            d_aprox = Modulo<double>(abs(x-mid_size.first) ,abs( y - mid_size.second));
            d = static_cast<int>(d_aprox);
            if (d > d_assigned_max) continue;
            I = p[x];
            // Search index in d= [d_aprox -1, d_aprox, d_aprox +1 ]
            it_begin = distance_indices.ind[d].begin();
            it_end = distance_indices.ind[d].end();
            ipair[0] = x;
            ipair[1] = y;
            it_found = find(it_begin, it_end, ipair);
            if (it_found!=it_end) intensities_at_distance[d].push_back(I);

            if (d!= 0){
                d--;
                it_begin = distance_indices.ind[d].begin();
                it_end = distance_indices.ind[d].end();
                it_found = find(it_begin, it_end, ipair);
                if (it_found!=it_end) intensities_at_distance[d].push_back(I);
                d++; //Restore old d for next step
            }
            if (d!= d_max){
                d++;
                it_begin = distance_indices.ind[d].begin();
                it_end = distance_indices.ind[d].end();
                it_found = find(it_begin, it_end, ipair);
                if (it_found!=it_end) intensities_at_distance[d].push_back(I);
            }
            // debug_counts++;
        }
    }
    // cout << "debug_counts: " << debug_counts << endl;
    return intensities_at_distance;
}

void SAXSsim::Show(){
    imshow("Input Image"  , I_   );
    if(!log_flag_) logDFT(dftMat_);
    normalize(dftMat_, dftMat_, 0, 1, NORM_MINMAX, -1, Mat());
    imshow("DFT:Magnitude", dftMat_);
    waitKey(0);
    destroyWindow("Input Image");
    destroyWindow("DFT:Magnitude");
}
void SAXSsim::SaveImage(cv::Mat & img, std::string & output_file){
    if(!log_flag_) logDFT(img);
    normalize(img, img, 0, 1, NORM_MINMAX, -1, Mat());
    img.convertTo(img, CV_8UC3, 255.0);
    imwrite(output_file, img);
}
#ifdef ENABLE_PARALLEL
SAXSsim::intensities_vector & SAXSsim::ParallelIntensityFromDistanceVector(){
    cout << "Calculating intensities in parallel" << endl;
    intensities_at_distance.resize(distance_indices.ind.size());
    int d_max = intensities_at_distance.size() - 1;

    // create intensities_at_distance for each thread.
    vector<intensities_vector> p_intensities;
    int Nthreads  = num_threads_;
    p_intensities.resize(Nthreads);
    for (int N    = 0;  N < Nthreads; N++ ){
        p_intensities[N].assign(intensities_at_distance.begin(), intensities_at_distance.end());
    }
    // int debug_counts{0};

    for( int x = 0; x < distance_indices.Nx; x++){
        #pragma omp parallel for
        for( int y = 0; y < distance_indices.Ny/2 +1; y++){
            int th = omp_get_thread_num();
            // stringstream buf;
            // buf << "Thread:" << th << ": " <<  x << ", " << y << endl;
            float *p;
            double I{0}, d_aprox{0};
            int  d{0} ;
            auto it_begin = distance_indices.ind[0].begin(),
                 it_end = distance_indices.ind[0].end(),
                 it_found = it_end;
            index_pair ipair{};
            p = dftMat_.ptr<float>(y);
            I = p[x];
            // d_aprox = Modulo<double>(static_cast<double>(x) - mid_size.first, static_cast<double>(y) - mid_size.second);
            d_aprox = Modulo<double>(abs(x - mid_size.first), abs(y - mid_size.second));
            // Search index in d= [d_aprox -1, d_aprox, d_aprox +1 ]
            d = static_cast<int>(d_aprox);
            if (d > d_assigned_max) continue;
            it_begin = distance_indices.ind[d].begin();
            it_end = distance_indices.ind[d].end();
            ipair[0] = x;
            ipair[1] = y;
            it_found = find(it_begin, it_end, ipair);
            if (it_found!=it_end) p_intensities[th][d].push_back(I);

            if (d!= 0){
                d--;
                it_begin = distance_indices.ind[d].begin();
                it_end = distance_indices.ind[d].end();
                it_found = find(it_begin, it_end, ipair);
                if (it_found!=it_end) p_intensities[th][d].push_back(I);
                d++; //Restore old d for next step
            }
            if (d!= d_max){
                d++;
                it_begin = distance_indices.ind[d].begin();
                it_end = distance_indices.ind[d].end();
                it_found = find(it_begin, it_end, ipair);
                if (it_found!=it_end) p_intensities[th][d].push_back(I);
            }
            // #pragma omp critical
            // cout << buf.rdbuf()<<endl;
        }
            // debug_counts++;
    }
    // cout << "debug_counts: " << debug_counts << endl;
    // MERGE p_intensities into intensities_at_distance
    for (int N    = 0;  N < num_threads_; N++ ){
        for (int _d = 0 ; _d < d_max + 1; _d++){
            auto it_end     = intensities_at_distance[_d].end();
            auto it_p_begin = p_intensities[N][_d].begin();
            auto it_p_end = p_intensities[N][_d].end();
            intensities_at_distance[_d].insert(it_end, it_p_begin, it_p_end);
        }
    }
    return intensities_at_distance;
}
#endif
