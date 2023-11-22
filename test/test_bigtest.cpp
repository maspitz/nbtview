#include <gtest/gtest.h>

#include <fstream>
#include <vector>

#include "Tag.hpp"
#include "nbtview.hpp"

using namespace nbtview;

class BigTest : public ::testing::Test {
  protected:
    Tag root_tag;
    std::string root_name;

    virtual void SetUp() {
        const auto filename = "test_data/bigtest.nbt";
        std::ifstream bigtest_stream(filename);
        std::tie(root_name, root_tag) = read_binary(bigtest_stream);
    }
};

TEST_F(BigTest, Values) {
    EXPECT_TRUE(root_tag.is<nbtview::Compound>());
    EXPECT_EQ(root_name, "Level");
    EXPECT_EQ(root_tag["byteTest"].get<Byte>(), 127);
    EXPECT_EQ(root_tag["longTest"].get<Long>(), 9223372036854775807L);
    EXPECT_EQ(root_tag["shortTest"].get<Short>(), 32767);
    EXPECT_EQ(root_tag["stringTest"].get<String>(),
              "HELLO WORLD THIS IS A TEST STRING ÅÄÖ!");
    EXPECT_NEAR(root_tag["floatTest"].get<Float>(), 0.49823147, 1e-5);
    EXPECT_EQ(root_tag["intTest"].get<Int>(), 2147483647);
    EXPECT_NEAR(root_tag["doubleTest"].get<Double>(), 0.4931287132182315, 1e-9);
}

TEST_F(BigTest, NestedCompounds) {
    auto &nested = root_tag["nested compound test"];
    EXPECT_EQ(nested["egg"]["name"].get<String>(), "Eggbert");
    EXPECT_NEAR(nested["egg"]["value"].get<Float>(), 0.5, 1e-9);
    EXPECT_EQ(nested["ham"]["name"].get<String>(), "Hampus");
    EXPECT_NEAR(nested["ham"]["value"].get<Float>(), 0.75, 1e-9);
}

TEST_F(BigTest, ListOfLongs) {
    auto &list_long = root_tag["listTest (long)"].get<nbtview::List>();
    EXPECT_EQ(list_type(list_long), TypeCode::Long);
    EXPECT_EQ(list_long.size(), 5);
    auto longvals = std::vector<Long>{11, 12, 13, 14, 15};
    for (std::size_t i = 0; i < list_long.size(); ++i) {
        EXPECT_EQ(list_long[i].get<Long>(), longvals[i]);
    }
}

TEST_F(BigTest, ListOfCompounds) {
    auto &list_cmpd = root_tag["listTest (compound)"].get<nbtview::List>();
    EXPECT_EQ(list_type(list_cmpd), TypeCode::Compound);
    EXPECT_EQ(list_cmpd.size(), 2);
    auto &cmpd0 = list_cmpd[0].get<nbtview::Compound>();
    EXPECT_EQ(cmpd0["created-on"].get<Long>(), 1264099775885L);
    EXPECT_EQ(cmpd0["name"].get<String>(), "Compound tag #0");
    auto &cmpd1 = list_cmpd[1].get<nbtview::Compound>();
    EXPECT_EQ(cmpd1["created-on"].get<Long>(), 1264099775885L);
    EXPECT_EQ(cmpd1["name"].get<String>(), "Compound tag #1");
}

TEST_F(BigTest, ByteArray) {
    auto byte_array_name = "byteArrayTest "
                           "(the first 1000 values of (n*n*255+n*7)%100, "
                           "starting with n=0 (0, 62, 34, 16, 8, ...))";
    EXPECT_TRUE(root_tag.contains(byte_array_name));
    auto &byte_array = root_tag[byte_array_name].get<Byte_Array>();
    EXPECT_EQ(byte_array.size(), 1000);
    for (int n = 0; n < 1000; ++n) {
        EXPECT_EQ(byte_array[n], (n * n * 255 + n * 7) % 100);
    }
}
