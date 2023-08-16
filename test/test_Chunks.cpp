#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <fstream>
#include <vector>

#include "Region.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview: Test reading of chunk data from region file") {
    const auto filename = "test_data/r.0.0.mca";
    int region_x = 0;
    int region_z = 0;
    std::ifstream region_file(filename);
    nbt::Region reg;
    reg.ReadOffsets(region_file);
    reg.ReadTimestamps(region_file);
    SUBCASE("coordinate consistency") {
        for (int i = 0; i < nbt::Region::chunk_count; ++i) {
            auto chunk_offset = reg.get_offset(i);
            auto chunk_length = reg.get_length(i);
            if (chunk_length == 0) {
                continue;
            }
            auto chunk_data = reg.get_chunk_data(region_file, i);
            auto [root_name, root_tag] = nbt::read_binary(chunk_data);
            CHECK(std::holds_alternative<nbt::Compound>(root_tag));
            nbt::Compound &root = std::get<nbt::Compound>(root_tag);

            CHECK(root.contains<nbt::Compound>("Level"));
            nbt::Compound &level = root.get<nbt::Compound>("Level");

            CHECK(level.contains<nbt::Int>("xPos"));
            CHECK(level.contains<nbt::Int>("zPos"));
            nbt::Int xPos = level.get<nbt::Int>("xPos");
            nbt::Int zPos = level.get<nbt::Int>("zPos");
            CHECK(region_x == xPos >> 5);
            CHECK(region_z == zPos >> 5);

            // let local_x, local_z be chunk coordinates local to this region.
            nbt::Int local_x = xPos & 0x1f;
            nbt::Int local_z = zPos & 0x1f;
            CHECK(i == (local_z * 32 + local_x));
            CHECK(local_x == (i & 0x1f));
            CHECK(local_z == (i >> 5));
        }
    }
}
