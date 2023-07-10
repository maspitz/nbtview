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

template <> struct StringMaker<std::vector<unsigned char>> {
    static String convert(const std::vector<unsigned char> &in) {
        std::ostringstream oss;

        oss << "[";
        oss << std::hex << std::setfill('0');
        for (typename std::vector<unsigned char>::const_iterator it =
                 in.begin();
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
}
