#include "nbtview.hpp"
#include <algorithm>
#include <bit>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>

namespace nbtview {

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    Tag::Type tag_type, const std::string &tag_name) {

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

auto EndOfInput = std::runtime_error("Unexpected end of input data");

template <typename Tag_Struct, typename Payload_Type>
std::unique_ptr<Tag_Struct> make_tag_struct(std::string_view name,
                                            BinaryScanner &s) {
    auto payload = s.get_value<Payload_Type>();
    if (payload == std::nullopt) {
        throw EndOfInput;
    }
    return std::make_unique<Tag_Struct>(name, payload.value());
}

template <typename Tag_Array, typename Element_Type>
std::unique_ptr<Tag_Array> make_tag_array(std::string_view name,
                                          BinaryScanner &s) {
    auto array_view = s.get_array_view<Element_Type>();
    if (array_view == std::nullopt) {
        throw EndOfInput;
    }
    return std::make_unique<Tag_Array>(name, array_view.value());
}

std::unique_ptr<String_Tag> make_tag_string(std::string_view name,
                                            BinaryScanner &s) {
    auto payload = s.get_string_view();
    if (payload == std::nullopt) {
        throw EndOfInput;
    }
    return std::make_unique<String_Tag>(name, payload.value());
}

std::unique_ptr<List_Tag> make_tag_list(std::string_view name,
                                        BinaryScanner &s) {
    auto list_type = s.get_value<int8_t>();
    auto list_length = s.get_value<int32_t>();
    if (list_type == std::nullopt || list_length == std::nullopt) {
        throw EndOfInput;
    }
    auto ltype = static_cast<Tag::Type>(list_type.value());
    auto list_tag = std::make_unique<List_Tag>(name);
    list_tag->data.reserve(list_length.value());
    for (int i = 0; i < list_length.value(); ++i) {
        list_tag->data.emplace_back(make_typed_tag(ltype, Tag::empty_name, s));
    }
    return list_tag;
}

std::unique_ptr<Compound_Tag> make_tag_compound(std::string_view name,
                                                BinaryScanner &s) {
    auto compound_tag = std::make_unique<Compound_Tag>(name);
    while (true) {
        auto next_type = s.peek_value<int8_t>();
        if (next_type == std::nullopt) {
            throw EndOfInput;
        }
        // TODO next: store payload of compound tag in std::map.
        // read type and name of elements in this loop
        // use std::map::try_emplace (or similar)
        // to insert a make_typed_tag into the map at the named location.
        // (note: this copies the name, among other features....)
        // (so maybe we'd rather avoid the map altogether???)
        // (what are the advantages of the map anyway?  lookup time?)
        // (QUERY: are all named tags actually elements of a compound tag???)
        if (static_cast<Tag::Type>(next_type.value()) == Tag::Type::End) {
            s.get_value<int8_t>(); // consumes the TAG_End to complete reading
                                   // the compound payload
            return compound_tag;
        }
        compound_tag->data.emplace_back(make_tag(s));
    }
}

std::unique_ptr<Tag> make_typed_tag(Tag::Type type, std::string_view name,
                                    BinaryScanner &s) {
    switch (type) {
    case Tag::Type::End:
        return std::make_unique<End_Tag>();
    case Tag::Type::Byte:
        return make_tag_struct<Byte_Tag, int8_t>(name, s);
    case Tag::Type::Short:
        return make_tag_struct<Short_Tag, int16_t>(name, s);
    case Tag::Type::Int:
        return make_tag_struct<Int_Tag, int32_t>(name, s);
    case Tag::Type::Long:
        return make_tag_struct<Long_Tag, int64_t>(name, s);
    case Tag::Type::Float:
        return make_tag_struct<Float_Tag, float>(name, s);
    case Tag::Type::Double:
        return make_tag_struct<Double_Tag, double>(name, s);
    case Tag::Type::Byte_Array:
        return make_tag_array<Byte_Array_Tag, int8_t>(name, s);
    case Tag::Type::String:
        return make_tag_string(name, s);
    case Tag::Type::List:
        return make_tag_list(name, s);
    case Tag::Type::Compound:
        return make_tag_compound(name, s);
    case Tag::Type::Int_Array:
        return make_tag_array<Int_Array_Tag, int32_t>(name, s);
    case Tag::Type::Long_Array:
        return make_tag_array<Long_Array_Tag, int64_t>(name, s);

    default:
        throw std::runtime_error("Unhandled tag type");
    }
}
// make_tag scans an NBT tag, including its type, name, and payload, and
// returns a unique_ptr to it.  If the end of the data is encountered,
// nullptr is returned instead.  As a side effect, make_tag advances the
// BinaryScanner to the end of the tag's payload.
std::unique_ptr<Tag> make_tag(BinaryScanner &s) {
    auto type = s.get_value<int8_t>();
    if (type == std::nullopt) {
        return nullptr;
    }
    if (static_cast<Tag::Type>(type.value()) == Tag::Type::End) {
        return std::make_unique<End_Tag>();
    }
    auto name = s.get_string_view();
    return make_typed_tag(static_cast<Tag::Type>(type.value()),
                          (name ? name.value() : Tag::empty_name), s);
}

template <typename InputIterator, typename OutputIterator>
InputIterator emplace_tag_typed(InputIterator input_start,
                                InputIterator input_stop, OutputIterator output,
                                Tag::Type type) {
    if (type == Tag::Type::Unspecified) {
        type = *(input_start++);
    }
    switch (type) {
    case Tag::Type::End:
        break;
    case Tag::Type::Byte:
        break;
    }
}

template <typename InputIterator, typename OutputIterator>
InputIterator emplace_tag(InputIterator input_start, InputIterator input_stop,
                          OutputIterator output) {
    return emplace_tag_typed(input_start, input_stop, output,
                             Tag::Type::Unspecified);
}

} // namespace nbtview

#ifdef TEST_NBTVIEW_INTERNALS
#include <catch2/catch_test_macros.hpp>

TEST_CASE("nbtview::BinaryScanner Integer Types") {
    auto v =
        std::vector<uint8_t>{0x17, 0x23, 0x01, 0xff, 0x01, 0x23, 0x45, 0x67};
    auto s = nbtview::BinaryScanner(v);
    auto a = s.get_value<int8_t>();
    auto b = s.get_value<int8_t>();
    auto c = s.get_value<int16_t>();
    auto d = s.get_value<int32_t>();
    auto e = s.get_value<int8_t>();

    REQUIRE(a != std::nullopt);
    REQUIRE(b != std::nullopt);
    REQUIRE(c != std::nullopt);
    REQUIRE(d != std::nullopt);
    REQUIRE(e == std::nullopt);
    REQUIRE(a.value() == 0x17);
    REQUIRE(b.value() == 0x23);
    REQUIRE(c.value() == 0x01ff);
    REQUIRE(d.value() == 0x01234567);
}

TEST_CASE("nbtview::BinaryScanner Floating Point Types") {
    auto v32 = std::vector<uint8_t>{0xc3, 0x78, 0xc0, 0x00};
    auto s32 = nbtview::BinaryScanner(v32);
    auto f32 = s32.get_value<float>();

    REQUIRE(f32 != std::nullopt);
    REQUIRE(f32.value() == -248.75);

    auto v64 =
        std::vector<uint8_t>{0x3f, 0xc9, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a};
    auto s64 = nbtview::BinaryScanner(v64);
    auto f64 = s64.get_value<double>();

    REQUIRE(f64 != std::nullopt);
    REQUIRE(f64.value() == 0.2);
}

#endif
