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
            std::vector<unsigned char>{0x0a, 0x00, 0x00, 0x00};
        auto [root_name, root_tag] = nbt::read_binary(v_empty_compound_tag);
        CHECK(root_tag.is<nbt::Compound>());
        CHECK(root_tag.size() == 0);
        CHECK(root_name == "");
    }

    SUBCASE("string tag 'foo' contains 'bar'") {
        auto v_foo_bar = std::vector<unsigned char>{
            0x0a, 0x00, 0x00, 0x08, 0x00, 0x03, 'f', 'o',
            'o',  0x00, 0x03, 'b',  'a',  'r',  0x00};
        auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
        CHECK(root_tag.is<nbt::Compound>());
        CHECK(root_tag.size() == 1);
        CHECK(root_name == "");

        REQUIRE(root_tag.contains("foo"));
        REQUIRE(root_tag["foo"].is<nbt::String>());
        CHECK(root_tag["foo"].get<nbt::String>() == "bar");
    }

    SUBCASE("read integer types from named compound tag") {
        auto v_foo_bar = std::vector<unsigned char>{
            0x0a, 0x00, 0x08, 't',  'e', 's', 't',  '_',  't',  'a',  'g',

            0x01, 0x00, 0x04, 'b',  'y', 't', 'e',  0x12,

            0x02, 0x00, 0x05, 's',  'h', 'o', 'r',  't',  0x12, 0x34,

            0x03, 0x00, 0x03, 'i',  'n', 't', 0x12, 0x34, 0x56, 0x78,

            0x04, 0x00, 0x04, 'l',  'o', 'n', 'g',  0x12, 0x03, 0x04, 0x05,
            0x06, 0x07, 0x08, 0x9a,

            0x00};
        auto [root_name, root_tag] = nbt::read_binary(v_foo_bar);
        CHECK(root_tag.is<nbt::Compound>());
        CHECK(root_tag.size() == 4);
        CHECK(root_name == "test_tag");

        REQUIRE(root_tag.contains("byte"));
        REQUIRE(root_tag["byte"].is<nbt::Byte>());
        CHECK(root_tag["byte"].get<nbt::Byte>() == 0x12);

        REQUIRE(root_tag.contains("short"));
        REQUIRE(root_tag["short"].is<nbt::Short>());
        CHECK(root_tag["short"].get<nbt::Short>() == 0x1234);

        REQUIRE(root_tag.contains("int"));
        REQUIRE(root_tag["int"].is<nbt::Int>());
        CHECK(root_tag["int"].get<nbt::Int>() == 0x12345678);

        REQUIRE(root_tag.contains("long"));
        REQUIRE(root_tag["long"].is<nbt::Long>());
        CHECK(root_tag["long"].get<nbt::Long>() == 0x120304050607089aL);
    }

    SUBCASE("read floating point types from nested compound tag") {
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
        REQUIRE(root_tag.is<nbt::Compound>());
        CHECK(root_tag.size() == 1);
        CHECK(root_name == "outertag");

        REQUIRE(root_tag.contains("innertag"));
        REQUIRE(root_tag["innertag"].is<nbt::Compound>());

        auto &inner_tag = root_tag["innertag"].get<nbt::Compound>();
        CHECK(inner_tag.size() == 2);

        REQUIRE(inner_tag.contains("Double"));
        REQUIRE(inner_tag["Double"].is<nbt::Double>());
        CHECK(inner_tag["Double"].get<nbt::Double>() == +0.2);

        REQUIRE(inner_tag.contains("Float"));
        REQUIRE(inner_tag["Float"].is<nbt::Float>());
        CHECK(inner_tag["Float"].get<nbt::Float>() == -248.75);

        CHECK(!inner_tag.contains("Not_Present"));
        CHECK(!inner_tag["Float"].is<nbt::Double>());
        CHECK(!inner_tag["Double"].is<nbt::Float>());
    }
}
