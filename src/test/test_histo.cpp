#include "gmock/gmock.h"
#include "histo.h"
#include <memory>
#include <iostream>
using namespace testing;
using namespace std;
using namespace histo;
struct HistoDouble : public ::testing::Test{
    static vector<double> data;
    static Histo<double>   h;
};
vector<double> HistoDouble::data{1.0,3.0,5.0};
Histo<double> HistoDouble::h(data, breaks_method::Scott);

TEST_F(HistoDouble, RangeMemberIsCorrect) {
    EXPECT_FLOAT_EQ(1.0, h.range.first);
    EXPECT_FLOAT_EQ(5.0, h.range.second);
}
TEST_F(HistoDouble, Breaks_AreCalculated_Automatically){
    EXPECT_EQ(2, h.bins);
    // EXPECT_FLOAT_EQ(2.3575135991909719, h.bin_width);
}
TEST_F(HistoDouble, HistMemberIsCorrect) {
    EXPECT_EQ(2, (int)h.counts.size());
    EXPECT_EQ(2, h.counts[0]);
    EXPECT_EQ(1, h.counts[1]);
}
TEST_F(HistoDouble, IndexFromValueWorks) {
    ASSERT_THROW( h.IndexFromValue(1.0 - numeric_limits<double>::epsilon()), histo_error);
    EXPECT_EQ(0, h.IndexFromValue(1.0));
    EXPECT_EQ(1, h.IndexFromValue(h.breaks[1])) << h.breaks[1];
    EXPECT_EQ(0, h.IndexFromValue(h.breaks[1] - numeric_limits<double>::epsilon())) << h.breaks[1];
    EXPECT_EQ(1, h.IndexFromValue(5.4));
    EXPECT_EQ(1, h.IndexFromValue(h.breaks[2]));
    EXPECT_EQ(1, h.IndexFromValue(h.breaks[2] + numeric_limits<double>::epsilon()) );
    EXPECT_EQ(1, h.IndexFromValue(h.breaks[2] + 2*numeric_limits<double>::epsilon()) );
    ASSERT_THROW(h.IndexFromValue(h.breaks[2] + 3*numeric_limits<double>::epsilon()), histo_error);
    ASSERT_THROW(h.IndexFromValue(10), histo_error);
}
// struct HistoInt : public ::testing::Test{
//     static vector<int> data;
//     static Histo<int>   h;
// };
// vector<int> HistoInt::data{1,2,3,4,5};
// Histo<int> HistoInt::h(data);
// TEST_F(HistoInt, NoFloatExceptions) {
//     EXPECT_EQ(4, (int)h.counts.size());
//     // EXPECT_EQ(4.0/4,h.bin_width);
// }
// TEST_F(HistoInt, FirstBinIsEmpty) {
//     EXPECT_EQ(1, h.counts[0]);
//     for(unsigned int i=1; i!=h.counts.size();i++){
//         EXPECT_EQ(1,h.counts[i]) <<" i "<<i;
//     }
// }


