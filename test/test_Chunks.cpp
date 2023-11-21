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
    nbt::Region_File reg(filename);
    SUBCASE("coordinate consistency") {
        for (int i = 0; i < nbt::Region::chunk_count; ++i) {
            auto chunk_offset = reg.chunk_offset(i);
            auto chunk_length = reg.chunk_length(i);
            if (chunk_length == 0) {
                continue;
            }
            auto chunk_data = reg.get_chunk_data(i);
            auto [root_name, root_tag] = nbt::read_binary(chunk_data);
            CHECK(root_tag.is<nbt::Compound>());
            CHECK(root_tag.contains("Level"));
            auto &level = root_tag["Level"];
            CHECK(level.contains("xPos"));
            CHECK(level.contains("zPos"));
            CHECK(level["xPos"].is<nbt::Int>());
            CHECK(level["zPos"].is<nbt::Int>());
            nbt::Int xPos = level["xPos"].get<nbt::Int>();
            nbt::Int zPos = level["zPos"].get<nbt::Int>();
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
