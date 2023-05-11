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

    template <typename T> std::optional<T> get_int() {
        if (read_index + sizeof(T) > data.size())
            return std::nullopt;
        T read_value = load_big_endian<T>(&data[read_index]);
        read_index += sizeof(T);
        return read_value;
    }
};

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

TEST_CASE("nbtview::BinaryScanner") {
    auto v =
        std::vector<uint8_t>{0x17, 0x23, 0x01, 0xff, 0x01, 0x23, 0x45, 0x67};
    auto s = nbtview::BinaryScanner(v);
    auto a = s.get_int<int8_t>();
    auto b = s.get_int<int8_t>();
    auto c = s.get_int<int16_t>();
    auto d = s.get_int<int32_t>();
    auto e = s.get_int<int8_t>();

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
#endif
