#include <gtest/gtest.h>

#include <ios>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

#include "Tag.hpp"
#include "nbtview.hpp"

namespace nbt = nbtview;

void expect_serialized_bytes_eq(
    const nbt::Tag &tag, const std::string &tag_name,
    const std::vector<unsigned char> &expected_bytes) {
    std::ostringstream output(std::ios::binary);
    nbt::write_binary(tag, tag_name, output);
    std::string_view output_view = output.view();
    std::vector<unsigned char> actual_bytes(output_view.begin(),
                                            output_view.end());
    ASSERT_EQ(actual_bytes.size(), expected_bytes.size());
    for (size_t i = 0; i < actual_bytes.size(); ++i) {
        EXPECT_EQ(actual_bytes[i], expected_bytes[i])
            << "\tat output byte " << i;
    }
}

TEST(SerializerTest, CompoundEmptyUnnamed) {
    nbt::Tag empty_compound(nbt::Compound{});
    auto expected_bytes = std::vector<unsigned char>{0x0a, 0x00, 0x00, 0x00};
    expect_serialized_bytes_eq(empty_compound, "", expected_bytes);
}

TEST(SerializerTest, ByteTagUnnamed) {
    nbt::Tag byte_tag(nbt::Byte{0x17});
    auto expected_bytes = std::vector<unsigned char>{
        static_cast<unsigned char>(nbt::TypeCode::Byte), 0x00, 0x00, 0x17};
    expect_serialized_bytes_eq(byte_tag, "", expected_bytes);
}

TEST(SerializerTest, ByteTagNamed) {
    nbt::Tag byte_tag(nbt::Byte{0x17});
    auto expected_bytes = std::vector<unsigned char>{
        static_cast<unsigned char>(nbt::TypeCode::Byte),
        0x00,
        0x06,
        'M',
        'y',
        'B',
        'y',
        't',
        'e',
        0x17};
    expect_serialized_bytes_eq(byte_tag, "MyByte", expected_bytes);
}

TEST(SerializerTest, StringTagNamed) {
    nbt::Tag string_tag("Hello");
    auto expected_bytes = std::vector<unsigned char>{
        0x08, 0x00, 0x03, 'T', 'a', 'g', 0x00, 0x05, 'H', 'e', 'l', 'l', 'o'};
    expect_serialized_bytes_eq(string_tag, "Tag", expected_bytes);
}

TEST(SerializerTest, ListLongs) {
    nbt::Tag list_tag(nbt::List{});
    list_tag.push_back(nbt::Long(10));
    list_tag.push_back(nbt::Long(15));
    auto list_tag_name = "A";
    auto expected_bytes = std::vector<unsigned char>{
        0x09,                                           // TypeCode::List
        0x00, 0x01,                                     // Name length
        'A',                                            // "A"
        0x04,                                           // TypeCode::Long
        0x00, 0x00, 0x00, 0x02,                         // list length of 2
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, // 10L
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f  // 15L

    };
    expect_serialized_bytes_eq(list_tag, list_tag_name, expected_bytes);
}

TEST(SerializerTest, NestedCompounds) {
    nbt::Tag ham(nbt::Compound{});
    ham.emplace("Hampus", nbt::Float(0.75));
    auto v_ham = std::vector<unsigned char>{
        0x0a,                                       // Compound
        0x00, 0x03, 'h',  'a',  'm',                // "ham"
        0x05,                                       // Float
        0x00, 0x06, 'H',  'a',  'm', 'p', 'u', 's', // "Hampus"
        0x3f, 0x40, 0x00, 0x00,                     // 0.75 [IEEE 754]
        0x00                                        // End
    };
    expect_serialized_bytes_eq(ham, "ham", v_ham);

    nbt::Compound egg;
    egg.emplace("Eggbert", nbt::Float(0.50));
    auto v_egg = std::vector<unsigned char>{
        0x0a,                                            // Compound
        0x00, 0x03, 'e',  'g',  'g',                     // "egg"
        0x05,                                            // Float
        0x00, 0x07, 'E',  'g',  'g', 'b', 'e', 'r', 't', // "Eggbert"
        0x3f, 0x00, 0x00, 0x00,                          // 0.50
        0x00                                             // End
    };
    expect_serialized_bytes_eq(egg, "egg", v_egg);

    nbt::Compound nested;
    nested["ham"] = std::move(ham); // move assignment
    nested["egg"] = egg;            // copy assignment
    auto v_nested = std::vector<unsigned char>{
        0x0a,                                    // Compound
        0x00, 0x06, 'n', 'e', 's', 't', 'e', 'd' // "nested"
    };
    // Because we are using std::map with a std::string key,
    // Compounds will be serialized with their elements appearing
    // in lexicographic order.  That is, "egg" comes before "ham".
    v_nested.insert(v_nested.end(), v_egg.begin(), v_egg.end());
    v_nested.insert(v_nested.end(), v_ham.begin(), v_ham.end());
    v_nested.push_back(0x00); // End

    expect_serialized_bytes_eq(nested, "nested", v_nested);
}
