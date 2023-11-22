#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <vector>

#include "doctest.h"

#include "BinaryDeserializer.hpp"
#include "Tag.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

TEST_CASE("nbtview::BinaryDeserializer explicit compound tags") {
    SUBCASE("unnamed empty compound tag") {
        auto v_empty_compound_tag =
            std::vector<unsigned char>{0x0a, 0x00, 0x00, 0x00};

        nbt::BinaryDeserializer reader(v_empty_compound_tag.data(),
                                       v_empty_compound_tag.size());
        auto [root_name, root_data] = reader.deserialize();
        CHECK(root_name == "");
        CHECK(root_data.is<nbt::Compound>());
        CHECK(root_data.size() == 0);
    }

    SUBCASE("root string tag") {
        auto v_root_string_tag =
            std::vector<uint8_t>{0x08, 0x00, 0x03, 'T', 'a', 'g', 0x00,
                                 0x05, 'H',  'e',  'l', 'l', 'o'};
        nbt::BinaryDeserializer reader(v_root_string_tag.data(),
                                       v_root_string_tag.size());
        auto [root_name, root_data] = reader.deserialize();
        CHECK(root_name == "Tag");

        CHECK(root_data.is<nbt::String>());
        CHECK(root_data.get<nbt::String>() == "Hello");
    }
}
