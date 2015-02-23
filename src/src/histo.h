/*
Histogram. Basic struct to handle histograms.
Copyright (C) 2015 Pablo Hernandez

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HISTO_H_
#define HISTO_H_
#include <vector>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <cmath>
template <typename T, typename Container>
T variance_welford(const Container& xs)
{
    size_t N = 0;
    T M = 0, S = 0, Mprev = 0;
    for(auto x : xs) {
        ++N;
        Mprev = M;
        M += (x - Mprev) / N;
        S += (x - Mprev) * (x - M);
    }
    return S / (N-1);
}

template <typename T, typename PRECI = double>
struct Histo {
    Histo() = default;
    Histo( const std::vector<T> &input_array, int input_bins = 0 ):
    data{input_array},
    bins{input_bins}
    {
        auto range_ptr = std::minmax_element(data.begin(), data.end());
        range = std::make_pair(*range_ptr.first, *range_ptr.second);
        if (input_bins == 0) {
            auto p    = CalculateOptimalBinsAndWidth(data);
            bins      = p.first;
            bin_width = p.second;
        } else {
            bin_width = ( range.second - range.first ) / static_cast<PRECI>(bins);
        }

        FillHisto();
    };
    std::vector<T> & FillHisto(){
        // resize() hist to access the bins without push_backs.
        // reserve() only allocate memory, but capacity is not changed.
        hist.resize(bins);
        std::for_each(std::begin(hist), std::end(hist),
                [](T & vh){ vh = 0; });
        std::for_each(std::begin(data),std::end(data),
                [this](const T & v){
                    auto x = v / bin_width;
                    auto t = std::trunc( x ) ;
                    if (t==x && t!=range.first) hist[t - 1]++;
                    else hist[t]++;
                });
        return hist;
    };

    T* get_hist_ptr() { return hist.data;};

    std::vector<T> data;
    int bins{0};
    std::pair<T,T> range;
    PRECI bin_width;
    std::vector<T> hist;
    enum {Scotts = 0};
private:
    std::pair<int,PRECI> CalculateOptimalBinsAndWidth(const std::vector<T> & in, int method = Scotts){
        switch(method) {
        case Scotts:
             return ScottsMethod(in,range);
             break;
        default:
             throw std::runtime_error("No Method selected to calculate automatically bins and width");
        }
        return ScottsMethod(in, range);
    };

    std::pair<int,PRECI> ScottsMethod(const std::vector<T> &in, const std::pair<T,T> &rang){
        PRECI sigma = variance_welford<PRECI,std::vector<T>>(in);
        PRECI width  = 3.5 * sqrt(sigma) / static_cast<PRECI>( in.size() );
        int n    = std::ceil( (rang.second - rang.first) / width);
        n++;
        return std::make_pair (n,width);
    };
};


#endif
