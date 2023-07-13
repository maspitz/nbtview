#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <vector>

#include "BinaryDeserializer.hpp"
#include "BinaryScanner.hpp"
#include "Tag.hpp"
#include "doctest.h"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview::BinaryDeserializer explicit compound tags") {
    SUBCASE("unnamed empty compound tag") {
        auto v_empty_compound_tag =
            std::vector<uint8_t>{0x0a, 0x00, 0x00, 0x00};

        nbt::BinaryDeserializer reader(v_empty_compound_tag);
        auto root_data = reader.deserialize();
        auto root_name = root_data.first;
        CHECK(root_name == "");

        CHECK(std::holds_alternative<nbt::Compound>(root_data.second));
        auto &cmpd = std::get<nbt::Compound>(root_data.second);
        CHECK(cmpd.size() == 0);
    }

    SUBCASE("root string tag") {
        auto v_root_string_tag =
            std::vector<uint8_t>{0x08, 0x00, 0x03, 'T', 'a', 'g', 0x00,
                                 0x05, 'H',  'e',  'l', 'l', 'o'};
        nbt::BinaryDeserializer reader(v_root_string_tag);
        auto root_data = reader.deserialize();
        auto root_name = root_data.first;
        CHECK(root_name == "Tag");

        CHECK(std::holds_alternative<nbt::String>(root_data.second));
        auto str_data = std::get<nbt::String>(root_data.second);
        CHECK(str_data == "Hello");
    }
}
