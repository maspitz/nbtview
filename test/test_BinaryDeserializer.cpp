#include <gtest/gtest.h>

#include <vector>

#include "BinaryDeserializer.hpp"
#include "Tag.hpp"

namespace nbt = nbtview;

TEST(BinaryDeserializer, EmptyCompound) {
    auto v_empty_compound_tag =
        std::vector<unsigned char>{0x0a, 0x00, 0x00, 0x00};

    nbt::BinaryDeserializer reader(v_empty_compound_tag.data(),
                                   v_empty_compound_tag.size());
    auto [root_name, root_data] = reader.deserialize();
    EXPECT_EQ(root_name, "");
    EXPECT_TRUE(root_data.is<nbt::Compound>());
    EXPECT_EQ(root_data.size(), 0);
}
TEST(BinaryDeserializer, RootStringTag) {

    auto v_root_string_tag = std::vector<uint8_t>{
        0x08, 0x00, 0x03, 'T', 'a', 'g', 0x00, 0x05, 'H', 'e', 'l', 'l', 'o'};
    nbt::BinaryDeserializer reader(v_root_string_tag.data(),
                                   v_root_string_tag.size());
    auto [root_name, root_data] = reader.deserialize();
    EXPECT_EQ(root_name, "Tag");

    EXPECT_TRUE(root_data.is<nbt::String>());
    EXPECT_EQ(root_data.get<nbt::String>(), "Hello");
}
