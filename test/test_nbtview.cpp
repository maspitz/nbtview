#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <vector>

#include "doctest.h"

#include "Compound.hpp"
#include "List.hpp"
#include "nbtview.hpp"

TEST_CASE("nbtview::Compound_Tag explicit compound tags") {
    SUBCASE("empty compound tag") {
        auto v_empty_compound_tag =
            std::vector<uint8_t>{0x0a, 0x00, 0x00, 0x00};
        auto s = nbtview::BinaryScanner(v_empty_compound_tag);

        auto root_tag = nbtview::make_tag_root(s);
        CHECK(root_tag->data.size() == 0);
    }
    SUBCASE("string tag 'foo' contains 'bar'") {
        auto v_foo_bar =
            std::vector<uint8_t>{0x0a, 0x00, 0x00, 0x08, 0x00, 0x03, 'f', 'o',
                                 'o',  0x00, 0x03, 'b',  'a',  'r',  0x00};
        auto s = nbtview::BinaryScanner(v_foo_bar);
        auto root_tag = nbtview::make_tag_root(s);
        REQUIRE(root_tag->data.size() == 1);

        auto str_foo = root_tag->get_String("foo");
        REQUIRE(str_foo);
        CHECK(*str_foo == "bar");
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
        auto s = nbtview::BinaryScanner(v_foo_bar);
        auto root_tag = nbtview::make_tag_root(s);
        REQUIRE(root_tag->data.size() == 4);

        REQUIRE(root_tag->get_Byte("byte"));
        CHECK(root_tag->get_Byte("byte").value() == 0x12);
        REQUIRE(root_tag->get_Short("short"));
        CHECK(root_tag->get_Short("short").value() == 0x1234);
        REQUIRE(root_tag->get_Int("int"));
        CHECK(root_tag->get_Int("int").value() == 0x12345678);
        REQUIRE(root_tag->get_Long("long"));
        CHECK(root_tag->get_Long("long").value() == 0x120304050607089aL);
    }

    SUBCASE("read floating point types from nested compound tag") {
        auto v_foo_bar = std::vector<uint8_t>{
            0x0a, 0x00, 0x08, 'o',  'u',  't', 'e', 'r', 't',  'a',  'g',

            0x0a, 0x00, 0x08, 'i',  'n',  'n', 'e', 'r', 't',  'a',  'g',

            0x05, 0x00, 0x05, 'F',  'l',  'o', 'a', 't', 0xc3, 0x78, 0xc0, 0x00,

            0x06, 0x00, 0x06, 'D',  'o',  'u', 'b', 'l', 'e',  0x3f, 0xc9, 0x99,
            0x99, 0x99, 0x99, 0x99, 0x9a,

            0x00,

            0x00};
        auto s = nbtview::BinaryScanner(v_foo_bar);
        auto root_tag = nbtview::make_tag_root(s);
        REQUIRE(root_tag->data.size() == 1);

        auto inner_tag = root_tag->get_Compound("innertag");
        REQUIRE(inner_tag != nullptr);

        auto double_tag = inner_tag->get_Double("Double");

        REQUIRE(double_tag);

        CHECK(*double_tag == +0.2);
        CHECK(double_tag.value() == +0.2);

        auto float_tag = inner_tag->get_Float("Float");

        REQUIRE(float_tag);

        CHECK(float_tag.value() == -248.75);

        float_tag = inner_tag->get_Float("Not_Present");

        REQUIRE(!float_tag);

        float_tag = inner_tag->get_Float("Double");

        REQUIRE(!float_tag);

        double_tag = inner_tag->get_Double("Float");

        REQUIRE(!double_tag);
    }
}
