#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <fstream>
#include <vector>

#include "Region.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview: Region container methods") {
    const auto filename = "test_data/r.0.0.mca";
    nbt::Region_File reg(filename);
    SUBCASE("offset data") {
        CHECK(reg.chunk_offset(0) == 0x02);
        CHECK(reg.chunk_offset(1) == 0x1b);
        CHECK(reg.chunk_offset(2) == 0x34);
        CHECK(reg.chunk_offset(3) == 0x00);
    }
    SUBCASE("length data") {
        CHECK(reg.chunk_length(0) == 0x01);
        CHECK(reg.chunk_length(1) == 0x01);
        CHECK(reg.chunk_length(2) == 0x01);
        CHECK(reg.chunk_length(3) == 0x00);
    }
    SUBCASE("timestamp data") {
        CHECK(reg.chunk_timestamp(0) == 0x600a1b75);
        CHECK(reg.chunk_timestamp(1) == 0x6004c6d7);
        CHECK(reg.chunk_timestamp(2) == 0x6004c6d7);
        CHECK(reg.chunk_timestamp(3) == 0x00000000);
    }
}
