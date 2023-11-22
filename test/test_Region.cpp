#include <gtest/gtest.h>

#include "Region.hpp"

namespace nbt = nbtview;

// Test for offset data
TEST(RegionTest, OffsetData) {
    const auto filename = "test_data/r.0.0.mca";
    nbt::Region_File reg(filename);

    EXPECT_EQ(reg.chunk_offset(0), 0x02);
    EXPECT_EQ(reg.chunk_offset(1), 0x1b);
    EXPECT_EQ(reg.chunk_offset(2), 0x34);
    EXPECT_EQ(reg.chunk_offset(3), 0x00);
}

// Test for length data
TEST(RegionTest, LengthData) {
    const auto filename = "test_data/r.0.0.mca";
    nbt::Region_File reg(filename);

    EXPECT_EQ(reg.chunk_length(0), 0x01);
    EXPECT_EQ(reg.chunk_length(1), 0x01);
    EXPECT_EQ(reg.chunk_length(2), 0x01);
    EXPECT_EQ(reg.chunk_length(3), 0x00);
}

// Test for timestamp data
TEST(RegionTest, TimestampData) {
    const auto filename = "test_data/r.0.0.mca";
    nbt::Region_File reg(filename);

    EXPECT_EQ(reg.chunk_timestamp(0), 0x600a1b75);
    EXPECT_EQ(reg.chunk_timestamp(1), 0x6004c6d7);
    EXPECT_EQ(reg.chunk_timestamp(2), 0x6004c6d7);
    EXPECT_EQ(reg.chunk_timestamp(3), 0x00000000);
}
