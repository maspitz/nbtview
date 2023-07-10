#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <ios>
#include <sstream>
#include <string_view>
#include <vector>

#include "Compound.hpp"
#include "List.hpp"
#include "Serializer.hpp"
#include "Tag.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

namespace doctest {

template <> struct StringMaker<std::vector<char>> {
    static String convert(const std::vector<char> &in) {
        std::ostringstream oss;

        oss << "[";
        oss << std::hex << std::setfill('0');
        for (typename std::vector<char>::const_iterator it = in.begin();
             it != in.end(); ++it)
            oss << "0x" << std::setw(2) << static_cast<unsigned int>(*it)
                << ", ";
        oss << "]";

        return oss.str().c_str();
    }
};
} // namespace doctest

std::vector<char> stream_chars(std::ostringstream &stream) {
    auto outview = stream.view();
    return std::vector<char>(outview.begin(), outview.end());
}

TEST_CASE("nbtview::Serializer functions") {
    std::ostringstream output(std::ios::binary);

    SUBCASE("Serialize simple value tags") {

        SUBCASE("Byte tag (unnamed)") {
            nbt::Serializer::serialize(nbt::Byte(0x17), "", output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{static_cast<char>(nbt::TypeCode::Byte),
                                    0x00, 0x00, 0x17});
        }

        SUBCASE("Byte tag (named)") {
            nbt::Serializer::serialize(nbt::Byte(0x17), "MyByte", output);
            CHECK(stream_chars(output) ==
                  std::vector<char>{static_cast<char>(nbt::TypeCode::Byte),
                                    0x00, 0x06, 'M', 'y', 'B', 'y', 't', 'e',
                                    0x17});
        }
    }

    SUBCASE("Serialize string tags") {
        nbt::Tag empty_compound(nbt::String{"Hello"});
        nbt::Serializer::serialize(empty_compound, "Tag", output);
        auto v_root_string_tag =
            std::vector<char>{0x08, 0x00, 0x03, 'T', 'a', 'g', 0x00,
                              0x05, 'H',  'e',  'l', 'l', 'o'};

        CHECK(stream_chars(output) == v_root_string_tag);
    }

    SUBCASE("Serialize compound tags") {
        SUBCASE("Empty compound tag (unnamed)") {
            nbt::Tag empty_compound(nbt::Compound{});
            nbt::Serializer::serialize(empty_compound, "", output);
            auto v_empty_compound_tag =
                std::vector<char>{0x0a, 0x00, 0x00, 0x00};
            CHECK(stream_chars(output) == v_empty_compound_tag);
        }
    }
}
