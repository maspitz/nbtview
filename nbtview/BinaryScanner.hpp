/**
 * @file BinaryScanner.hpp
 * @brief Read big-endian numeric values, arrays, and strings
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef BINARYSCANNER_H_
#define BINARYSCANNER_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace nbtview {

class UnexpectedEndOfInputException : public std::runtime_error {
  public:
    UnexpectedEndOfInputException()
        : std::runtime_error("Unexpected end of input") {}
};

// BinaryScanner scans and reads big-endian binary data.
class BinaryScanner {
  private:
    const std::vector<unsigned char> read_data;
    const unsigned char *const read_begin;
    const unsigned char *const read_end;
    const unsigned char *read_ptr;
    const size_t data_size;

  public:
    BinaryScanner(std::vector<unsigned char> bytes)
        : read_data(std::move(bytes)), read_begin(read_data.data()),
          read_end(read_begin + read_data.size()), read_ptr(read_begin),
          data_size(read_end - read_begin) {}

    template <typename T> T get_value();

    size_t read_pos() { return read_ptr - read_begin; }

    std::string get_string() {
        auto str_len = get_value<uint16_t>();
        if (read_pos() + str_len > data_size) {
            throw UnexpectedEndOfInputException();
        }
        std::string ret_str(reinterpret_cast<const char *>(read_ptr), str_len);
        read_ptr += str_len;
        return ret_str;
    }

    template <typename Element_Type> std::vector<Element_Type> get_vector();

    template <typename Element_Type> std::vector<Element_Type> get_vector2();
};

namespace detail {
    template <typename T>
    void swap_endian(T &val,
                     typename std::enable_if<std::is_arithmetic<T>::value,
                                             std::nullptr_t>::type = nullptr) {
        auto ptr = reinterpret_cast<std::uint8_t *>(&val);
        std::array<std::uint8_t, sizeof(T)> raw_src, raw_dst;

        for (std::size_t i = 0; i < sizeof(T); ++i)
            raw_src[i] = ptr[i];

        std::reverse_copy(raw_src.begin(), raw_src.end(), raw_dst.begin());

        for (std::size_t i = 0; i < sizeof(T); ++i)
            ptr[i] = raw_dst[i];
    }

    template <typename T>
    [[nodiscard]] T load_big_endian(
        const unsigned char *const buf) noexcept requires std::is_trivial_v<T> {
        T res;
        std::reverse_copy(buf, buf + sizeof res,
                          reinterpret_cast<unsigned char *>(&res));
        return res;
    }

} // namespace detail

template <typename T> T BinaryScanner::get_value() {
    if (read_pos() + sizeof(T) > data_size) {
        throw UnexpectedEndOfInputException();
    }
    T read_value = detail::load_big_endian<T>(read_ptr);
    read_ptr += sizeof(T);
    return read_value;
}

// swap_endian in-place method
template <typename Element_Type>
std::vector<Element_Type> BinaryScanner::get_vector() {
    auto array_length = get_value<int32_t>();
    if (array_length < 0) {
        throw std::runtime_error("Negative array length encountered");
    }
    auto data_length = array_length * sizeof(Element_Type);
    if (read_pos() + data_length > data_size) {
        throw UnexpectedEndOfInputException();
    }

    auto span_start = reinterpret_cast<const Element_Type *>(read_ptr);
    read_ptr += data_length;
    auto span_stop = reinterpret_cast<const Element_Type *>(read_ptr);
    auto vec = std::vector<Element_Type>(span_start, span_stop);
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        detail::swap_endian(*it);
    }
    return vec;
}

// load_big_endian method
template <typename Element_Type>
std::vector<Element_Type> BinaryScanner::get_vector2() {
    auto array_length = get_value<int32_t>();
    if (array_length < 0) {
        throw std::runtime_error("Negative array length encountered");
    }
    auto read_stop = read_ptr + sizeof(Element_Type) * array_length;
    if (read_stop > read_end) {
        throw UnexpectedEndOfInputException();
    }

    auto vec = std::vector<Element_Type>();
    vec->reserve(array_length);
    for (; read_ptr < read_stop; read_ptr += sizeof(Element_Type)) {
        vec->push_back(detail::load_big_endian<Element_Type>(read_ptr));
    }
    return vec;
}

} // namespace nbtview

#endif // BINARYSCANNER_H_
