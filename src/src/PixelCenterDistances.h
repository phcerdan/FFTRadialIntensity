#ifndef PIXEL_CENTER_DISTANCES_H_
#define PIXEL_CENTER_DISTANCES_H_
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
struct PixelCenterDistances {
    PixelCenterDistances() = default;
    using index_pair        = std::array<unsigned int, 2>;
    using index_pair_vector = std::vector<index_pair>;
    unsigned int max;
    std::vector<index_pair_vector> ind;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive( max, ind );
    }
};
#endif

