#include <gtest/gtest.h>

#include <vector>

#include "Tag.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST(NbtviewTest, EmptyCompoundTag) {
    auto v_empty_compound_tag =
        std::vector<unsigned char>{0x0a, 0x00, 0x00, 0x00};
    auto [root_name, root_tag] = nbt::read_binary(v_empty_compound_tag);
    EXPECT_TRUE(root_tag.is<nbt::Compound>());
    EXPECT_EQ(root_tag.size(), 0);
    EXPECT_EQ(root_name, "");
}

TEST(NbtviewTest, StringTagData) {
    auto v_foo_bar =
        std::vector<unsigned char>{0x0a, 0x00, 0x00, 0x08, 0x00, 0x03, 'f', 'o',
                                   'o',  0x00, 0x03, 'b',  'a',  'r',  0x00};
    auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
    EXPECT_TRUE(root_tag.is<nbt::Compound>());
    EXPECT_EQ(root_tag.size(), 1);
    EXPECT_EQ(root_name, "");

    EXPECT_TRUE(root_tag.contains("foo"));
    EXPECT_TRUE(root_tag["foo"].is<nbt::String>());
    EXPECT_EQ(root_tag["foo"].get<nbt::String>(), "bar");
}

TEST(NbtviewTest, CompoundWithInteger) {
    auto v_foo_bar = std::vector<unsigned char>{
        0x0a, 0x00, 0x08, 't',  'e', 's', 't',  '_',  't',  'a',  'g',

        0x01, 0x00, 0x04, 'b',  'y', 't', 'e',  0x12,

        0x02, 0x00, 0x05, 's',  'h', 'o', 'r',  't',  0x12, 0x34,

        0x03, 0x00, 0x03, 'i',  'n', 't', 0x12, 0x34, 0x56, 0x78,

        0x04, 0x00, 0x04, 'l',  'o', 'n', 'g',  0x12, 0x03, 0x04, 0x05,
        0x06, 0x07, 0x08, 0x9a,

        0x00};
    auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
    EXPECT_TRUE(root_tag.is<nbt::Compound>());
    EXPECT_EQ(root_tag.size(), 4);
    EXPECT_EQ(root_name, "test_tag");

    EXPECT_TRUE(root_tag.contains("byte"));
    EXPECT_TRUE(root_tag["byte"].is<nbt::Byte>());
    EXPECT_EQ(root_tag["byte"].get<nbt::Byte>(), 0x12);

    EXPECT_TRUE(root_tag.contains("short"));
    EXPECT_TRUE(root_tag["short"].is<nbt::Short>());
    EXPECT_EQ(root_tag["short"].get<nbt::Short>(), 0x1234);

    EXPECT_TRUE(root_tag.contains("int"));
    EXPECT_TRUE(root_tag["int"].is<nbt::Int>());
    EXPECT_EQ(root_tag["int"].get<nbt::Int>(), 0x12345678);

    EXPECT_TRUE(root_tag.contains("long"));
    EXPECT_TRUE(root_tag["long"].is<nbt::Long>());
    EXPECT_EQ(root_tag["long"].get<nbt::Long>(), 0x120304050607089aL);
}

TEST(NbtviewTest, CompoundWithFloatingPoint) {
    auto v_foo_bar = std::vector<unsigned char>{
        0x0a, 0x00, 0x08, 'o',  'u',  't',  'e', 'r', 't',  'a',  'g',

        0x0a, 0x00, 0x08, 'i',  'n',  'n',  'e', 'r', 't',  'a',  'g',

        0x05, 0x00, 0x05, 'F',  'l',  'o',  'a', 't', 0xc3, 0x78, 0xc0,
        0x00, // +0.2

        0x06, 0x00, 0x06, 'D',  'o',  'u',  'b', 'l', 'e',  0x3f, 0xc9,
        0x99, 0x99, 0x99, 0x99, 0x99, 0x9a, // -248.75

        0x00,

        0x00};

    auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
    EXPECT_TRUE(root_tag.is<nbt::Compound>());
    EXPECT_EQ(root_tag.size(), 1);
    EXPECT_EQ(root_name, "outertag");

    EXPECT_TRUE(root_tag.contains("innertag"));
    EXPECT_TRUE(root_tag["innertag"].is<nbt::Compound>());

    auto &inner_tag = root_tag["innertag"].get<nbt::Compound>();
    EXPECT_EQ(inner_tag.size(), 2);

    EXPECT_TRUE(inner_tag.contains("Double"));
    EXPECT_TRUE(inner_tag["Double"].is<nbt::Double>());
    EXPECT_EQ(inner_tag["Double"].get<nbt::Double>(), +0.2);

    EXPECT_TRUE(inner_tag.contains("Float"));
    EXPECT_TRUE(inner_tag["Float"].is<nbt::Float>());
    EXPECT_EQ(inner_tag["Float"].get<nbt::Float>(), -248.75);

    EXPECT_TRUE(!inner_tag.contains("Not_Present"));
    EXPECT_TRUE(!inner_tag["Float"].is<nbt::Double>());
    EXPECT_TRUE(!inner_tag["Double"].is<nbt::Float>());
}
