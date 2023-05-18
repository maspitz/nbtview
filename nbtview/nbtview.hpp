// nbtview.hpp

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <algorithm>
#include <format>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace nbtview {

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

    const Tag::Type type;
    std::optional<std::string_view> name;
    Tag(Tag::Type type, std::optional<std::string_view> name)
        : type(type), name(name) {}
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

struct End_Tag : public Tag {
    End_Tag() : Tag(Tag::Type::End, std::nullopt) {}
    std::string to_string() { return "End_Tag"; }
};

struct Byte_Tag : public Tag {
    int8_t data;
    Byte_Tag(std::optional<std::string_view> name, int8_t data)
        : Tag(Tag::Type::Byte, name), data(data) {}
    std::string to_string() {
        return std::format("'{}': {}B", (name ? name.value() : ""), data);
    }
};

struct Short_Tag : public Tag {
    int16_t data;
    Short_Tag(std::optional<std::string_view> name, int16_t data)
        : Tag(Tag::Type::Short, name), data(data) {}
    std::string to_string() {
        return std::format("'{}': {}S", (name ? name.value() : ""), data);
    }
};

struct Int_Tag : public Tag {
    int32_t data;
    Int_Tag(std::optional<std::string_view> name, int32_t data)
        : Tag(Tag::Type::Int, name), data(data) {}
    std::string to_string() {
        return std::format("'{}': {}", (name ? name.value() : ""), data);
    }
};

struct Long_Tag : public Tag {
    int64_t data;
    Long_Tag(std::optional<std::string_view> name, int64_t data)
        : Tag(Tag::Type::Long, name), data(data) {}
    std::string to_string() {
        return std::format("'{}': {}L", (name ? name.value() : ""), data);
    }
};

struct Float_Tag : public Tag {
    static_assert(std::numeric_limits<double>::is_iec559,
                  "IEEE 754 floating point");
    static_assert(sizeof(float) == 4, "float type is 32-bit");
    float data;
    Float_Tag(std::optional<std::string_view> name, float data)
        : Tag(Tag::Type::Float, name), data(data) {}
    std::string to_string() {
        return std::format("'{}': {}F", (name ? name.value() : ""), data);
    }
};

struct Double_Tag : public Tag {
    static_assert(std::numeric_limits<double>::is_iec559,
                  "IEEE 754 floating point");
    static_assert(sizeof(double) == 8, "double type is 64-bit");
    double data;
    Double_Tag(std::optional<std::string_view> name, double data)
        : Tag(Tag::Type::Double, name), data(data) {}
    std::string to_string() {
        return std::format("'{}': {}D", (name ? name.value() : ""), data);
    }
};

struct Byte_Array_Tag : public Tag {
    std::span<int8_t> data;
    Byte_Array_Tag(std::optional<std::string_view> name, std::span<int8_t> data)
        : Tag(Tag::Type::Byte_Array, name), data(data) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': [") : "'': [");
        for (auto element : data) {
            oss << std::to_string(element) << "B, ";
        }
        oss << "]";
        return oss.str();
    }
};

struct String_Tag : public Tag {
    std::string_view data;
    String_Tag(std::optional<std::string_view> name, std::string_view data)
        : Tag(Tag::Type::String, name), data(data) {}
    std::string to_string() {
        return ((name) ? ("'" + std::string(name.value()) + "': ") : "'': '") +
               std::string(data) + "'";
    }
};

struct List_Tag : public Tag {
    std::vector<std::unique_ptr<Tag>> data;
    List_Tag(std::optional<std::string_view> name)
        : Tag(Tag::Type::List, name) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': List [")
                       : "'': List [");
        for (auto tag_it = data.begin(); tag_it != data.end(); ++tag_it) {
            oss << (*tag_it)->to_string() << ", ";
        }
        oss << "]";
        return oss.str();
    }
};

struct Compound_Tag : public Tag {
    std::vector<std::unique_ptr<Tag>> data;
    Compound_Tag(std::optional<std::string_view> name)
        : Tag(Tag::Type::Compound, name) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': Compound [")
                       : "'': Compound [");
        for (auto tag_it = data.begin(); tag_it != data.end(); ++tag_it) {
            oss << (*tag_it)->to_string() << ", ";
        }
        oss << "]";
        return oss.str();
    }
};

struct Int_Array_Tag : public Tag {
    std::span<int32_t> data;
    Int_Array_Tag(std::optional<std::string_view> name, std::span<int32_t> data)
        : Tag(Tag::Type::Int_Array, name), data(data) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': [") : "'': [");
        for (auto element : data) {
            oss << std::to_string(element) << ", ";
        }
        oss << "]";
        return oss.str();
    }
};

struct Long_Array_Tag : public Tag {
    std::span<int64_t> data;
    Long_Array_Tag(std::optional<std::string_view> name,
                   std::span<int64_t> data)
        : Tag(Tag::Type::Long_Array, name), data(data) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': [") : "'': [");
        for (auto element : data) {
            oss << std::to_string(element) << "L, ";
        }
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

std::unique_ptr<Tag> make_typed_tag(Tag::Type type,
                                    std::optional<std::string_view> name,
                                    BinaryScanner &s);

std::unique_ptr<Tag> make_tag(BinaryScanner &s);

} // namespace nbtview

#endif // NBTVIEW_H_
