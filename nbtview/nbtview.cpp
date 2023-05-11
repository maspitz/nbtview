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

} // namespace nbtview
