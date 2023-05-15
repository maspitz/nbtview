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
                    tagtype tag_type, const std::string &tag_name) {

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

// BinaryScanner scans and reads big-endian binary data.

class BinaryScanner {
  public:
    BinaryScanner(const std::span<uint8_t> &data) : data(data), read_index(0) {}
    std::span<uint8_t> data;
    size_t read_index;

    template <typename T>
    [[nodiscard]] T load_big_endian(
        const uint8_t *const buf) noexcept requires std::is_trivial_v<T> {
        T res;
        std::reverse_copy(buf, buf + sizeof res,
                          reinterpret_cast<uint8_t *>(&res));
        return res;
    }

    template <typename T> std::optional<T> get_value() {
        if (read_index + sizeof(T) > data.size()) {
            read_index = data.size();
            return std::nullopt;
        }
        T read_value = load_big_endian<T>(&data[read_index]);
        read_index += sizeof(T);
        return read_value;
    }

    template <typename T> std::optional<T> peek_value() {
        if (read_index + sizeof(T) > data.size()) {
            return std::nullopt;
        }
        T read_value = load_big_endian<T>(&data[read_index]);
        return read_value;
    }

    std::optional<std::string_view> get_string_view() {
        auto str_len = get_value<uint16_t>();
        if (str_len == std::nullopt ||
            read_index + str_len.value() > data.size()) {
            read_index = data.size();
            return std::nullopt;
        }
        if (str_len.value() == 0) {
            return std::nullopt;
        }
        auto sv = std::string_view(reinterpret_cast<char *>(&data[read_index]),
                                   str_len.value());
        read_index += str_len.value();
        return sv;
    }

    template <typename Element_Type>
    std::optional<std::span<Element_Type>> get_array_view() {
        auto array_length = get_value<int32_t>();
        if (array_length == std::nullopt) {
            return std::nullopt;
        }
        if (array_length.value() < 0) {
            throw std::runtime_error("Negative array length encountered");
        }
        if (read_index + sizeof(Element_Type) * array_length.value() >
            data.size()) {
            read_index = data.size();
            return std::nullopt;
        }
        auto span_start = reinterpret_cast<Element_Type *>(&data[read_index]);
        auto span_stop = span_start + array_length.value();
        return std::span<Element_Type>(span_start, span_stop);
    }
};

template <typename Tag_Struct, typename Payload_Type>
std::unique_ptr<Tag_Struct>
make_tag_struct(std::optional<std::string_view> name, BinaryScanner &s) {
    auto payload = s.get_value<Payload_Type>();
    if (payload == std::nullopt) {
        throw EndOfInput;
    }
    return std::make_unique<Tag_Struct>(name, payload.value());
}

template <typename Tag_Array, typename Element_Type>
std::unique_ptr<Tag_Array> make_tag_array(std::optional<std::string_view> name,
                                          BinaryScanner &s) {
    auto array_view = s.get_array_view<Element_Type>();
    if (array_view == std::nullopt) {
        throw EndOfInput;
    }
    return std::make_unique<Tag_Array>(name, array_view.value());
}

std::unique_ptr<String_Tag>
make_tag_string(std::optional<std::string_view> name, BinaryScanner &s) {
    auto payload = s.get_string_view();
    if (payload == std::nullopt) {
        throw EndOfInput;
    }
    return std::make_unique<String_Tag>(name, payload.value());
}

std::unique_ptr<Tag> make_typed_tag(tagtype type,
                                    std::optional<std::string_view> name,
                                    BinaryScanner &s);

std::unique_ptr<List_Tag> make_tag_list(std::optional<std::string_view> name,
                                        BinaryScanner &s) {
    auto list_type = s.get_value<int8_t>();
    auto list_length = s.get_value<int32_t>();
    if (list_type == std::nullopt || list_length == std::nullopt) {
        throw EndOfInput;
    }
    auto ltype = static_cast<tagtype>(list_type.value());
    auto list_tag = std::make_unique<List_Tag>(name);
    list_tag->data.reserve(list_length.value());
    for (int i = 0; i < list_length.value(); ++i) {
        list_tag.data.emplace_back(make_typed_tag(ltype, std::nullopt, s));
    }
    return list_tag;
}

std::unique_ptr<Tag> make_typed_tag(tagtype type,
                                    std::optional<std::string_view> name,
                                    BinaryScanner &s) {
    switch (type) {
    case tagtype::TAG_End:
        return std::make_unique<End_Tag>();
    case tagtype::TAG_Byte:
        return make_tag_struct<Byte_Tag, int8_t>(name, s);
    case tagtype::TAG_Short:
        return make_tag_struct<Short_Tag, int16_t>(name, s);
    case tagtype::TAG_Int:
        return make_tag_struct<Int_Tag, int32_t>(name, s);
    case tagtype::TAG_Long:
        return make_tag_struct<Long_Tag, int64_t>(name, s);
    case tagtype::TAG_Float:
        return make_tag_struct<Float_Tag, float>(name, s);
    case tagtype::TAG_Double:
        return make_tag_struct<Double_Tag, double>(name, s);
    case tagtype::TAG_Byte_Array:
        return make_tag_array<Byte_Array_Tag, int8_t>(name, s);
    case tagtype::TAG_String:
        return make_tag_string(name, s);
    case tagtype::TAG_List:
        return make_tag_list(name, s);
    case tagtype::TAG_Int_Array:
        return make_tag_array<Int_Array_Tag, int32_t>(name, s);
    case tagtype::TAG_Long_Array:
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
    if (static_cast<tagtype>(type.value()) == tagtype::TAG_End) {
        return std::make_unique<End_Tag>();
    }
    auto name = s.get_string_view();
    return make_typed_tag(static_cast<tagtype>(type.value()), name, s);
}

template <typename InputIterator, typename OutputIterator>
InputIterator emplace_tag_typed(InputIterator input_start,
                                InputIterator input_stop, OutputIterator output,
                                tagtype type) {
    if (type == tagtype::TAG_Unspecified) {
        type = *(input_start++);
    }
    switch (type) {
    case tagtype::TAG_End:
        break;
    case tagtype::TAG_Byte:
        break;
    }
}

template <typename InputIterator, typename OutputIterator>
InputIterator emplace_tag(InputIterator input_start, InputIterator input_stop,
                          OutputIterator output) {
    return emplace_tag_typed(input_start, input_stop, output,
                             tagtype::TAG_Unspecified);
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
