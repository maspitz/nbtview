#include <gtest/gtest.h>

#include <fstream>
#include <string>

#include "Region.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST(ChunkTest, CoordinateConsistency) {
    const std::string filename = "test_data/r.0.0.mca";
    nbt::Region_File reg(filename);
    int region_x = 0;
    int region_z = 0;
    for (int i = 0; i < nbt::Region::chunk_count; ++i) {
        auto chunk_offset = reg.chunk_offset(i);
        auto chunk_length = reg.chunk_length(i);
        if (chunk_length == 0) {
            continue;
        }
        auto chunk_data = reg.get_chunk_data(i);
        auto [root_name, root_tag] = nbt::read_binary(chunk_data);
        EXPECT_TRUE(root_tag.is<nbt::Compound>());
        EXPECT_TRUE(root_tag.contains("Level"));
        auto &level = root_tag["Level"];
        EXPECT_TRUE(level.contains("xPos"));
        EXPECT_TRUE(level.contains("zPos"));
        EXPECT_TRUE(level["xPos"].is<nbt::Int>());
        EXPECT_TRUE(level["zPos"].is<nbt::Int>());
        nbt::Int xPos = level["xPos"].get<nbt::Int>();
        nbt::Int zPos = level["zPos"].get<nbt::Int>();
        EXPECT_EQ(region_x, xPos >> 5);
        EXPECT_EQ(region_z, zPos >> 5);

        // let local_x, local_z be chunk coordinates local to this region.
        nbt::Int local_x = xPos & 0x1f;
        nbt::Int local_z = zPos & 0x1f;
        EXPECT_EQ(i, (local_z * 32 + local_x));
        EXPECT_EQ(local_x, (i & 0x1f));
        EXPECT_EQ(local_z, (i >> 5));
    }
}
