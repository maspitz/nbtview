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

    SUBCASE("Serialize list tags") {
        SUBCASE("List tag of longs") {
            nbt::Tag list_tag(nbt::List{nbt::TypeCode::Long});
            nbt::List &myList = std::get<nbt::List>(list_tag);
            myList.push_back(nbt::Long(10));
            myList.push_back(nbt::Long(15));
            nbt::Serializer::serialize(list_tag, "A", output);
            auto v_empty_compound_tag = std::vector<char>{
                0x09,                   // TypeCode::List
                0x00, 0x01,             // Name length
                'A',                    // "A"
                0x04,                   // TypeCode::Long
                0x00, 0x00, 0x00, 0x02, // list length of 2
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, // 10L
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f  // 15L

            };
            CHECK(stream_chars(output) == v_empty_compound_tag);
        }
    }

    SUBCASE("Serialize compound tags") {
        SUBCASE("Empty compound tag (unnamed)") {
            nbt::Compound empty_compound;
            nbt::Serializer::serialize(empty_compound, "", output);
            auto v_empty_compound_tag =
                std::vector<char>{0x0a, 0x00, 0x00, 0x00};
            CHECK(stream_chars(output) == v_empty_compound_tag);
        }
        nbt::Compound ham;
        ham.emplace("Hampus", nbt::Float(0.75));
        nbt::Compound egg;
        egg.emplace("Eggbert", nbt::Float(0.50));
        auto v_ham = std::vector<char>{
            0x0a,                                       // Compound
            0x00, 0x03, 'h',  'a',  'm',                // "ham"
            0x05,                                       // Float
            0x00, 0x06, 'H',  'a',  'm', 'p', 'u', 's', // "Hampus"
            0x3f, 0x40, 0x00, 0x00,                     // 0.75 [IEEE 754]
            0x00                                        // End
        };
        auto v_egg = std::vector<char>{
            0x0a,                                            // Compound
            0x00, 0x03, 'e',  'g',  'g',                     // "egg"
            0x05,                                            // Float
            0x00, 0x07, 'E',  'g',  'g', 'b', 'e', 'r', 't', // "Eggbert"
            0x3f, 0x00, 0x00, 0x00,                          // 0.50
            0x00                                             // End
        };
        SUBCASE("Compound tag with float #1") {
            nbt::Serializer::serialize(ham, "ham", output);
            CHECK(stream_chars(output) == v_ham);
        }
        SUBCASE("Compound tag with float #2") {
            nbt::Serializer::serialize(egg, "egg", output);
            CHECK(stream_chars(output) == v_egg);
        }
        SUBCASE("Nested Compound tag") {
            nbt::Compound nested;
            nested.put("ham", std::move(ham));
            nested.put("egg", std::move(egg));
            auto v_nested = std::vector<char>{
                0x0a,                                    // Compound
                0x00, 0x06, 'n', 'e', 's', 't', 'e', 'd' // "nested"
            };
            // Because we are using std::map with a std::string key,
            // Compounds will be serialized with their elements appearing
            // in lexicographic order.  That is, "egg" comes before "ham".
            v_nested.insert(v_nested.end(), v_egg.begin(), v_egg.end());
            v_nested.insert(v_nested.end(), v_ham.begin(), v_ham.end());
            v_nested.push_back(0x00); // End

            nbt::Serializer::serialize(nested, "nested", output);
            CHECK(stream_chars(output) == v_nested);
        }
    }
}
