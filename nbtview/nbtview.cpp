#include "nbtview.hpp"
#include <algorithm>
#include <bit>
#include <cstdint>
#include <span>
#include <type_traits>

namespace nbtview {

std::unique_ptr<Compound> make_tag_compound(BinaryScanner &s);
std::unique_ptr<List> make_tag_list(BinaryScanner &s);

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    TypeCode tag_type, const std::string &tag_name) {

    auto loc = nbt_start;
    const char byte_1 = static_cast<char>(tag_type);
    const char byte_2 = tag_name.length() / 16;
    const char byte_3 = tag_name.length() % 16;
    while (1) {
        loc = std::search(loc, nbt_stop, tag_name.begin(), tag_name.end());
        if (loc == nbt_stop) {
            return nbt_stop;
        }
        if (loc - nbt_start >= 3 && *(loc - 3) == byte_1 &&
            *(loc - 2) == byte_2 && *(loc - 1) == byte_3 &&
            tag_name.length() < nbt_stop - loc) {
            return loc + tag_name.length();
        }
        loc++;
    }
}

Tag::payload_type decode_payload(TypeCode type, BinaryScanner &s) {
    switch (type) {
    case TypeCode::End:
        throw std::runtime_error("Unexpected End Tag");
    case TypeCode::Byte:
        return s.get_value<Byte>();
    case TypeCode::Short:
        return s.get_value<Short>();
    case TypeCode::Int:
        return s.get_value<Int>();
    case TypeCode::Long:
        return s.get_value<Long>();
    case TypeCode::Float:
        return s.get_value<Float>();
    case TypeCode::Double:
        return s.get_value<Double>();
    case TypeCode::Byte_Array:
        return s.get_vector<Byte>();
    case TypeCode::String:
        return s.get_string();
    case TypeCode::List:
        return make_tag_list(s);
    case TypeCode::Compound:
        return make_tag_compound(s);
    case TypeCode::Int_Array:
        return s.get_vector<Int>();
    case TypeCode::Long_Array:
        return s.get_vector<Long>();
    default:
        throw std::runtime_error("Unhandled tag type");
    }
    return 0;
}

std::unique_ptr<List> make_tag_list(BinaryScanner &s) {
    auto list_tag = std::make_unique<List>();
    auto list_type = static_cast<TypeCode>(s.get_value<int8_t>());
    auto list_length = s.get_value<int32_t>();
    list_tag->data.reserve(list_length);
    for (int32_t i = 0; i < list_length; ++i) {
        list_tag->data.emplace_back(decode_payload(list_type, s));
    }
    return list_tag;
}

// TODO: return name of root tag if desired
std::unique_ptr<Compound> make_tag_root(BinaryScanner &s) {
    auto compound_tag = std::make_unique<Compound>();
    auto root_type = static_cast<TypeCode>(s.get_value<int8_t>());
    if (root_type != TypeCode::Compound) {
        throw std::runtime_error("Root tag is not a compound tag");
    }
    auto root_name = s.get_string();
    return make_tag_compound(s);
}

std::unique_ptr<Compound> make_tag_compound(BinaryScanner &s) {
    auto compound_tag = std::make_unique<Compound>();
    auto next_type = static_cast<TypeCode>(s.get_value<int8_t>());
    std::string next_name;
    while (next_type != TypeCode::End) {
        next_name = s.get_string();
        compound_tag->data.emplace(next_name, decode_payload(next_type, s));
        next_type = static_cast<TypeCode>(s.get_value<int8_t>());
    }
    return compound_tag;
}

} // namespace nbtview

#ifdef TEST_NBTVIEW_INTERNALS
//#include <catch2/catch_test_macros.hpp>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("nbtview::detail::BinaryScanner Integer Types") {
    auto v =
        std::vector<uint8_t>{0x17, 0x23, 0x01, 0xff, 0x01, 0x23, 0x45, 0x67};
    auto s = nbtview::BinaryScanner(v);
    auto a = s.get_value<int8_t>();
    auto b = s.get_value<int8_t>();
    auto c = s.get_value<int16_t>();
    auto d = s.get_value<int32_t>();
    CHECK_THROWS_AS(s.get_value<int8_t>(), std::runtime_error);

    REQUIRE(a == 0x17);
    REQUIRE(b == 0x23);
    REQUIRE(c == 0x01ff);
    REQUIRE(d == 0x01234567);
}

TEST_CASE("nbtview::detail::BinaryScanner Floating Point Types") {
    auto v32 = std::vector<uint8_t>{0xc3, 0x78, 0xc0, 0x00};
    auto s32 = nbtview::BinaryScanner(v32);
    auto f32 = s32.get_value<float>();

    REQUIRE(f32 == -248.75);

    auto v64 =
        std::vector<uint8_t>{0x3f, 0xc9, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a};
    auto s64 = nbtview::BinaryScanner(v64);
    auto f64 = s64.get_value<double>();

    REQUIRE(f64 == 0.2);
}

#endif
