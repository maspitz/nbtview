#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
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

        auto value_bar = root_tag->get<std::string>("foo");
        CHECK(value_bar == "bar");
    }
}
