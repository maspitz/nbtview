/**
 * @file BinaryWriter.hpp
 * @brief Write big-endian numeric values, arrays, and strings
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef BINARYWRITER_H_
#define BINARYWRITER_H_

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <vector>

namespace nbtview {

// BinaryWriter encodes and writes big-endian binary data to a given output
// stream

class BinaryWriter {
  public:
    template <typename T>
    typename std::enable_if<std::is_trivial_v<T> && !std::is_array_v<T>,
                            void>::type static write(T value,
                                                     std::ostream &output) {
        char *bytePointer = reinterpret_cast<char *>(&value);
#ifndef TARGET_BIG_ENDIAN
        // If targeted architecture is not big endian, do byte swapping and
        // output.
        std::reverse(bytePointer, bytePointer + sizeof(T));
#endif
        output.write(bytePointer, sizeof(T));
    }

    static void write_string(std::string_view s, std::ostream &output) {
        write(static_cast<uint16_t>(s.size()), output);
        output.write(s.data(), s.size());
    }

    template <typename T>
    typename std::enable_if<std::is_trivial_v<T>, void>::
        type static write_vector(const std::vector<T> &values,
                                 std::ostream &output) {
        write(static_cast<int32_t>(values.size()), output);
        for (auto val : values) {
            write(val, output);
        }
    }
};

} // namespace nbtview

#endif // BINARYWRITER_H_
