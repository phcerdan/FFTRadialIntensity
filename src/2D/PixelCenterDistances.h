#ifndef PIXEL_CENTER_DISTANCES_H_
#define PIXEL_CENTER_DISTANCES_H_
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
struct PixelCenterDistances {
    PixelCenterDistances() = default;
    using index_pair        = std::array<int, 2>;
    using index_pair_vector = std::vector<index_pair>;
    int Nx;
    int Ny;
    // int max;
    std::vector<index_pair_vector> ind;

    template<class Archive>
    void serialize(Archive & archive, const int)
    {
        archive & Nx & Ny & ind ;
    }
};
#endif

