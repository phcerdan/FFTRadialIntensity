#include "gmock/gmock.h"
#include "histo.h"
#include <memory>
#include <iostream>
using namespace testing;
using namespace std;

struct HistoDoubleTest : public ::testing::Test{
    static vector<double> data;
    static Histo<double>   h;
};
vector<double> HistoDoubleTest::data{1.0,2.5,5.0};
Histo<double> HistoDoubleTest::h(data,0);

TEST_F(HistoDoubleTest,DataMemberIsFilled){
    ASSERT_EQ(data[0],h.data[0]);
}
TEST_F(HistoDoubleTest, RangeMemberIsCorrect) {
    EXPECT_FLOAT_EQ(1.0, h.range.first);
    EXPECT_FLOAT_EQ(5.0, h.range.second);
}
TEST_F(HistoDoubleTest, BinsAndWidth_AreCalculated_Automatically){
    EXPECT_EQ(3, h.bins);
    EXPECT_FLOAT_EQ(2.3575135991909719, h.bin_width);
}
TEST_F(HistoDoubleTest, HistMemberIsCorrect) {
    EXPECT_EQ(3, h.hist.size());
    EXPECT_EQ(1, h.hist[0]);
    EXPECT_EQ(1, h.hist[1]);
    EXPECT_EQ(1, h.hist[2]);
}

struct HistoIntTest : public ::testing::Test{
    static vector<int> data;
    static Histo<int>   h;
};
vector<int> HistoIntTest::data{1,2,3,4,5};
Histo<int> HistoIntTest::h(data,4);
TEST_F(HistoIntTest, NoFloatExceptions) {
    EXPECT_EQ(4, h.hist.size());
    EXPECT_EQ(4.0/4,h.bin_width);
}
TEST_F(HistoIntTest, FirstBinIsEmpty) {
    EXPECT_EQ(1, h.hist[0]);
    for(unsigned int i=1; i!=h.hist.size();i++){
        EXPECT_EQ(1,h.hist[i]) <<" i "<<i;
    }
}


