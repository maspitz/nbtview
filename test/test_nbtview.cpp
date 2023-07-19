#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <variant>
#include <vector>

#include "doctest.h"

#include "Tag.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview::Compound_Tag explicit compound tags") {
    SUBCASE("empty compound tag") {
        auto v_empty_compound_tag =
            std::vector<uint8_t>{0x0a, 0x00, 0x00, 0x00};
        auto [root_name, root_tag] = nbt::read_binary(v_empty_compound_tag);
        auto &root_compound = std::get<nbt::Compound>(root_tag);
        CHECK(root_compound.size() == 0);
        CHECK(root_name == "");
    }
    SUBCASE("string tag 'foo' contains 'bar'") {
        auto v_foo_bar =
            std::vector<uint8_t>{0x0a, 0x00, 0x00, 0x08, 0x00, 0x03, 'f', 'o',
                                 'o',  0x00, 0x03, 'b',  'a',  'r',  0x00};
        auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
        auto &root_compound = std::get<nbt::Compound>(root_tag);
        CHECK(root_compound.size() == 1);
        CHECK(root_name == "");

        REQUIRE(root_compound.contains<nbt::String>("foo"));
        CHECK(root_compound.get<nbt::String>("foo") == "bar");
    }

    SUBCASE("read integer types from named compound tag") {
        auto v_foo_bar = std::vector<uint8_t>{
            0x0a, 0x00, 0x08, 't',  'e', 's', 't',  '_',  't',  'a',  'g',

            0x01, 0x00, 0x04, 'b',  'y', 't', 'e',  0x12,

            0x02, 0x00, 0x05, 's',  'h', 'o', 'r',  't',  0x12, 0x34,

            0x03, 0x00, 0x03, 'i',  'n', 't', 0x12, 0x34, 0x56, 0x78,

            0x04, 0x00, 0x04, 'l',  'o', 'n', 'g',  0x12, 0x03, 0x04, 0x05,
            0x06, 0x07, 0x08, 0x9a,

            0x00};
        auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
        auto &root_compound = std::get<nbt::Compound>(root_tag);
        CHECK(root_compound.size() == 4);
        CHECK(root_name == "test_tag");

        REQUIRE(root_compound.contains<nbt::Byte>("byte"));
        CHECK(root_compound.get<nbt::Byte>("byte") == 0x12);

        REQUIRE(root_compound.contains<nbt::Short>("short"));
        CHECK(root_compound.get<nbt::Short>("short") == 0x1234);

        REQUIRE(root_compound.contains<nbt::Int>("int"));
        CHECK(root_compound.get<nbt::Int>("int") == 0x12345678);

        REQUIRE(root_compound.contains<nbt::Long>("long"));
        CHECK(root_compound.get<nbt::Long>("long") == 0x120304050607089aL);
    }

    SUBCASE("read floating point types from nested compound tag") {
        auto v_foo_bar = std::vector<uint8_t>{
            0x0a, 0x00, 0x08, 'o',  'u',  't',  'e', 'r', 't',  'a',  'g',

            0x0a, 0x00, 0x08, 'i',  'n',  'n',  'e', 'r', 't',  'a',  'g',

            0x05, 0x00, 0x05, 'F',  'l',  'o',  'a', 't', 0xc3, 0x78, 0xc0,
            0x00, // +0.2

            0x06, 0x00, 0x06, 'D',  'o',  'u',  'b', 'l', 'e',  0x3f, 0xc9,
            0x99, 0x99, 0x99, 0x99, 0x99, 0x9a, // -248.75

            0x00,

            0x00};

        auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
        auto &root_compound = std::get<nbt::Compound>(root_tag);
        CHECK(root_compound.size() == 1);
        CHECK(root_name == "outertag");

        REQUIRE(root_compound.contains<nbt::Compound>("innertag"));

        auto &inner_tag = root_compound.get<nbt::Compound>("innertag");
        CHECK(inner_tag.size() == 2);

        REQUIRE(inner_tag.contains<nbt::Double>("Double"));
        CHECK(inner_tag.get<nbt::Double>("Double") == +0.2);

        REQUIRE(inner_tag.contains<nbt::Float>("Float"));
        CHECK(inner_tag.get<nbt::Float>("Float") == -248.75);

        CHECK(!inner_tag.contains<nbt::Float>("Not_Present"));
        CHECK(!inner_tag.contains<nbt::Double>("Float"));
        CHECK(!inner_tag.contains<nbt::Float>("Double"));
    }
}
