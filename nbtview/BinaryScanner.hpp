// BinaryScanner.hpp
#ifndef BINARYSCANNER_H_
#define BINARYSCANNER_H_

#include <algorithm>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace nbtview {

class UnexpectedEndOfInputException : public std::runtime_error {
  public:
    UnexpectedEndOfInputException()
        : std::runtime_error("Unexpected end of input") {}
};

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

    template <typename T> T get_value() {
        if (read_index + sizeof(T) > data.size()) {
            throw UnexpectedEndOfInputException();
        }
        T read_value = load_big_endian<T>(&data[read_index]);
        read_index += sizeof(T);
        return read_value;
    }

    std::string get_string() {
        auto str_len = get_value<uint16_t>();
        if (read_index + str_len > data.size()) {
            throw UnexpectedEndOfInputException();
        }
        auto str_start = read_index;
        read_index += str_len;
        return std::string(reinterpret_cast<char *>(&data[str_start]), str_len);
    }

    template <typename Element_Type>
    std::unique_ptr<std::vector<Element_Type>> get_vector() {
        auto array_length = get_value<int32_t>();
        if (array_length < 0) {
            throw std::runtime_error("Negative array length encountered");
        }
        if (read_index + sizeof(Element_Type) * array_length > data.size()) {
            throw UnexpectedEndOfInputException();
        }
        auto span_start = reinterpret_cast<Element_Type *>(&data[read_index]);
        auto span_stop = span_start + array_length;
        return std::make_unique<std::vector<Element_Type>>(span_start,
                                                           span_stop);
    }
};

} // namespace nbtview

#endif // BINARYSCANNER_H_
