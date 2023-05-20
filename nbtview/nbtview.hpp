// nbtview.hpp

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <algorithm>
#include <limits>
#include <map>
#include <memory>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace nbtview {

class List_Tag;
class Compound_Tag;

struct Tag {

    enum class Type : char {
        End = 0,
        Byte = 1,
        Short = 2,
        Int = 3,
        Long = 4,
        Float = 5,
        Double = 6,
        Byte_Array = 7,
        String = 8,
        List = 9,
        Compound = 10,
        Int_Array = 11,
        Long_Array = 12,
        Unspecified = 127
    };

    using payload_type =
        std::variant<int8_t, int16_t, int32_t, int64_t, float, double,
                     std::unique_ptr<std::vector<int8_t>>, std::string,
                     std::unique_ptr<List_Tag>, std::unique_ptr<Compound_Tag>,
                     std::unique_ptr<std::vector<int32_t>>,
                     std::unique_ptr<std::vector<int64_t>>>;

    const Tag::Type type;
    Tag(Tag::Type type) : type(type) {}
    virtual std::string to_string() = 0;
};

// Attempts to find a named tag in a range of NBT data by searching for its
// initial byte sequence.
//
// If the tag is found, an iterator to the beginning of its payload is returned.
// Otherwise, nbt_stop is returned.
//
// Caveats:
// (1) This method can be fooled if the tag's name appears in the
// payload of some other tag.  This is unlikely to happen by chance.
// (2) It is the calling code's responsibility to check that the remaining NBT
// data is large enough to contain the tag's expected payload.

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    Tag::Type tag_type, const std::string &tag_name);

struct List_Tag : public Tag {
    std::vector<payload_type> data;
    List_Tag() : Tag(Tag::Type::List) {}
    // throws std::out_of_range if idx out of range
    // throws std::bad_variant_access if element isn't type T.
    template <typename T> T get(int32_t idx) const {
        return std::get<T>(data.at(idx));
    }
    std::string to_string() {
        std::ostringstream oss;
        oss << "List [";
        oss << "LIST DATA NOT YET PRINTABLE"; // FIXME
        // for (auto tag_it = data.begin(); tag_it != data.end(); ++tag_it) {
        //     oss << (*tag_it)->to_string() << ", ";
        // }
        oss << "]";
        return oss.str();
    }
};

struct Compound_Tag : public Tag {
    std::map<std::string, payload_type> data;
    Compound_Tag() : Tag(Tag::Type::Compound) {}

    // throws std::out_of_range if name not present
    // throws std::bad_variant_access if element isn't type T.
    template <typename T> T get(const std::string &name) const {
        return std::get<T>(data.at(name));
    }

    std::string to_string() {
        std::ostringstream oss;
        oss << "Compound [";
        oss << "COMPOUND DATA NOT YET PRINTABLE"; // FIXME
        // for (auto tag_it = data.begin(); tag_it != data.end(); ++tag_it) {
        //     oss << (*tag_it)->to_string() << ", ";
        // }
        oss << "]";
        return oss.str();
    }
};

// emplace_tag reads the tag which begins at input, emplaces the constructed tag
// at the output, and returns an iterator advanced to the end of the tag that
// was read.
template <typename InputIterator, typename OutputIterator>
InputIterator emplace_tag(InputIterator input_start, InputIterator input_stop,
                          OutputIterator output);

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

    template <typename Element_Type> std::span<Element_Type> get_array_view() {
        auto array_length = get_value<int32_t>();
        if (array_length < 0) {
            throw std::runtime_error("Negative array length encountered");
        }
        if (read_index + sizeof(Element_Type) * array_length > data.size()) {
            throw UnexpectedEndOfInputException();
        }
        auto span_start = reinterpret_cast<Element_Type *>(&data[read_index]);
        auto span_stop = span_start + array_length;
        return std::span<Element_Type>(span_start, span_stop);
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

#endif // NBTVIEW_H_
