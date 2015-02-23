#include "gmock/gmock.h"
#include "histo.h"
#include <memory>
#include <iostream>
using namespace testing;
using namespace std;
//static parameters.
struct HistoDoubleTest : public ::testing::Test{
    static vector<double> data;
    static Histo<double>   h;
    // static void SetUpTestCase(){
    // };
};
vector<double> HistoDoubleTest::data{1.0,2.5,5.0};
Histo<double> HistoDoubleTest::h(data,0);// = make_shared<SAXSsim>(imgTiny);

TEST_F(HistoDoubleTest,DataMemberIsFilled){
    ASSERT_TRUE(h.data[0]==data[0]);
}
TEST_F(HistoDoubleTest, RangeMemberIsCorrect) {
    EXPECT_FLOAT_EQ(h.range.first, 1.0)  << h.range.first;
    EXPECT_FLOAT_EQ(h.range.second, 5.0) << h.range.second;
}
TEST_F(HistoDoubleTest, BinsAndWidth_AreCalculated_Automatically){
    ASSERT_TRUE(h.bins==2) << h.bins;
    EXPECT_FLOAT_EQ(h.bin_width,2.3575135991909719) <<  h.bin_width;
}
TEST_F(HistoDoubleTest, HistMemberIsCorrect) {
    ASSERT_EQ(h.hist[0], 1);
    ASSERT_EQ(h.hist[1], 2);
}
