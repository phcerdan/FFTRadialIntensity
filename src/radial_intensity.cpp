#include "radial_intensity.h"

namespace radial_intensity
{
FlattenIntensities AverageRadialFrequencyIntensities(const Intensities & intensities)
{
    FlattenIntensities intensities_mean(intensities.size());
    int d(0);
    for( const auto & iarray : intensities){
        double mean = (iarray.empty()) ? 0.0
            : std::accumulate(std::begin(iarray), std::end(iarray), 0.0) / static_cast<double>(iarray.size());
        intensities_mean[d] = mean;
        d++;
    }
    return intensities_mean;
}

void PrintIntensitiesHisto(const Intensities & intensities, const HistoBins & histo_bins, std::ostream & os)
{
    os << "Histogram: start-end: [ " << histo_bins.left << "," << histo_bins.right << " ] ; bin_width: " << histo_bins.bin_width << std::endl;
    int d = 0;
    for (auto &radius_index : intensities){
        double left = (d==0)? 0 : (d - 1) * histo_bins.bin_width;
        double right = (d==0)? 0 : (d) * histo_bins.bin_width;
        os << "Index: " << d << " ; Bin: [ " << left << ", " <<right << " ]" << std::endl;
        os << " #pixels at this radius: " << radius_index.size() << std::endl;
        os << " Intensities: [ ";
        for (auto &i : radius_index){
            os << i << ", ";
        }
        os << " ]" << std::endl;
        ++d;
    }
}


bool fileExists(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

void SaveRadialIntensityProfile(const FlattenIntensities & mean_intensities, const std::string &fname, const MetadataFields &metadata_fields)
{
    std::ofstream output_file (fname); // delete everything inside the file(default)

    if (!output_file.is_open()) {
        perror( ("Error creating IvsQ file in " + fname).c_str());
    }
    output_file << "# input_file=" << metadata_fields.name << std::endl;
    output_file << "# Nx=" << metadata_fields.size[0] << std::endl;
    output_file << "# Ny=" << metadata_fields.size[1] << std::endl;
    if(metadata_fields.size.size() == 3)
        output_file << "# Nz=" << metadata_fields.size[2] << std::endl;

    for (unsigned int i = 0; i < mean_intensities.size(); i++ ){
        output_file << i  << " " << mean_intensities[i] << std::endl;
    }

    if(output_file.bad()){
        perror( ("Error saving RadialIntensityProfile (I vs q) file in " + fname ).c_str());
    }
    output_file.close();
}
}
