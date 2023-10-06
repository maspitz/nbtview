/**
 * @file BinaryReader.hpp
 * @brief Read big-endian numeric values, arrays, and strings
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef BINARYREADER_H_
#define BINARYREADER_H_

#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace nbtview {

class UnexpectedEndOfInputException : public std::runtime_error {
  public:
    UnexpectedEndOfInputException()
        : std::runtime_error("Unexpected end of input") {}
};

// BinaryReader scans and reads big-endian binary data.
class BinaryReader {
  private:
    const unsigned char *buffer;
    size_t buffer_length;

  public:
    BinaryReader(const unsigned char *buffer, size_t buffer_length)
        : buffer(buffer), buffer_length(buffer_length) {}

    template <typename T> inline T read();

    inline std::string read_string(size_t str_len);

    template <typename T> inline std::vector<T> read_array(size_t vec_len);
};

template <typename T> inline T BinaryReader::read() {
    if (sizeof(T) > buffer_length) {
        throw UnexpectedEndOfInputException();
    }
    T result = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        result = static_cast<T>(*(buffer++)) | (result << 8);
    }
    buffer_length -= sizeof(T);
    return result;
}

template <> inline float BinaryReader::read<float>() {
    static_assert(sizeof(float) == 4, "float must have 32 bits");
    uint32_t fval = read<uint32_t>();
    return std::bit_cast<float>(fval);
}

template <> inline double BinaryReader::read<double>() {
    static_assert(sizeof(double) == 8, "double must have 64 bits");
    return std::bit_cast<double>(read<uint64_t>());
}

inline std::string BinaryReader::read_string(size_t str_len) {
    if (str_len > buffer_length) {
        throw UnexpectedEndOfInputException();
    }
    std::string result(reinterpret_cast<const char *>(buffer), str_len);
    buffer += str_len;
    buffer_length -= str_len;
    return result;
}

template <typename T>
inline std::vector<T> BinaryReader::read_array(size_t vec_len) {
    if (vec_len * sizeof(T) > buffer_length) {
        throw UnexpectedEndOfInputException();
    }
    std::vector<T> result;
    for (size_t i = 0; i < vec_len; ++i) {
        result.push_back(read<T>());
    }
    return result;
}

} // namespace nbtview

#endif // BINARYREADER_H_
