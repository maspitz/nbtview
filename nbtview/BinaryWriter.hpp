// BinaryWriter.hpp
#ifndef BINARYWRITER_H_
#define BINARYWRITER_H_

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <vector>

namespace nbtview {

// BinaryWriter writes big-endian binary data to a given output iterator.
// such as a std::back_inserter or a std::ostreambuf_iterator
template <class OutputIterator> class BinaryWriter {
  public:
    BinaryWriter(OutputIterator &stream) : output(stream) {}

    template <typename T>
    typename std::enable_if<std::is_trivial_v<T> && !std::is_array_v<T>,
                            void>::type
    write(T value) {
        char *bytePointer = reinterpret_cast<char *>(&value);
#ifndef TARGET_BIG_ENDIAN
        // If targeted architecture is not big endian, do byte swapping and
        // output.
        std::reverse(bytePointer, bytePointer + sizeof(T));
#endif
        // Otherwise, just output
        std::copy(bytePointer, bytePointer + sizeof(T), output);
    }

    void write_string(std::string_view s) {
        uint16_t s_len = s.length();
        write<uint16_t>(s_len);
        std::copy(s.begin(), s.end(), output);
    }

    template <typename T>
    typename std::enable_if<std::is_trivial_v<T>, void>::type
    write_vector(const std::vector<T> &values) {
        for (auto val : values) {
            write(val);
        }
    }

  private:
    OutputIterator &output;
};

} // namespace nbtview

#endif // BINARYWRITER_H_
