// nbtview.hpp

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <algorithm>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace nbtview {

enum class tagtype : char {
    TAG_End = 0,
    TAG_Byte = 1,
    TAG_Short = 2,
    TAG_Int = 3,
    TAG_Long = 4,
    TAG_Float = 5,
    TAG_Double = 6,
    TAG_Byte_Array = 7,
    TAG_String = 8,
    TAG_List = 9,
    TAG_Compound = 10,
    TAG_Int_Array = 11,
    TAG_Long_Array = 12,
    TAG_Unspecified = 127
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
                    tagtype tag_type, const std::string &tag_name);

struct Tag {
    const tagtype type;
    std::optional<std::string_view> name;
    Tag(tagtype type, std::optional<std::string_view> name)
        : type(type), name(name) {}
    virtual std::string to_string() = 0;
};

struct End_Tag : public Tag {
    End_Tag() : Tag(tagtype::TAG_End, std::nullopt) {}
    std::string to_string() { return "End_Tag"; }
};

struct Byte_Tag : public Tag {
    int8_t data;
    Byte_Tag(std::optional<std::string_view> name, int8_t data)
        : Tag(tagtype::TAG_Byte, name), data(data) {}
    std::string to_string() {
        return (name) ? ("'" + std::string(name.value()) + "': ")
                      : "'': " + std::to_string(data) + "B";
    }
};

struct Short_Tag : public Tag {
    int16_t data;
    Short_Tag(std::optional<std::string_view> name, int16_t data)
        : Tag(tagtype::TAG_Short, name), data(data) {}
    std::string to_string() {
        return (name) ? ("'" + std::string(name.value()) + "': ")
                      : "'': " + std::to_string(data) + "S";
    }
};

struct Int_Tag : public Tag {
    int32_t data;
    Int_Tag(std::optional<std::string_view> name, int32_t data)
        : Tag(tagtype::TAG_Int, name), data(data) {}
    std::string to_string() {
        return (name) ? ("'" + std::string(name.value()) + "': ")
                      : "'': " + std::to_string(data);
    }
};

struct Long_Tag : public Tag {
    int64_t data;
    Long_Tag(std::optional<std::string_view> name, int64_t data)
        : Tag(tagtype::TAG_Long, name), data(data) {}
    std::string to_string() {
        return (name) ? ("'" + std::string(name.value()) + "': ")
                      : "'': " + std::to_string(data) + "L";
    }
};

struct Float_Tag : public Tag {
    static_assert(std::numeric_limits<double>::is_iec559,
                  "IEEE 754 floating point");
    static_assert(sizeof(float) == 4, "float type is 32-bit");
    float data;
    Float_Tag(std::optional<std::string_view> name, float data)
        : Tag(tagtype::TAG_Float, name), data(data) {}
    std::string to_string() {
        return (name) ? ("'" + std::string(name.value()) + "': ")
                      : "'': " + std::to_string(data) + "F";
    }
};

struct Double_Tag : public Tag {
    static_assert(std::numeric_limits<double>::is_iec559,
                  "IEEE 754 floating point");
    static_assert(sizeof(double) == 8, "double type is 64-bit");
    double data;
    Double_Tag(std::optional<std::string_view> name, double data)
        : Tag(tagtype::TAG_Double, name), data(data) {}
    std::string to_string() {
        return (name) ? ("'" + std::string(name.value()) + "': ")
                      : "'': " + std::to_string(data) + "D";
    }
};

struct Byte_Array_Tag : public Tag {
    std::span<int8_t> data;
    Byte_Array_Tag(std::optional<std::string_view> name, std::span<int8_t> data)
        : Tag(tagtype::TAG_Byte_Array, name), data(data) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': [") : "'': [");
        for (auto element : data) {
            oss << element << "B, ";
        }
        oss << "]";
        return oss.str();
    }
};

struct String_Tag : public Tag {
    std::string_view data;
    String_Tag(std::optional<std::string_view> name, std::string_view data)
        : Tag(tagtype::TAG_String, name), data(data) {}
    std::string to_string() {
        return ((name) ? ("'" + std::string(name.value()) + "': ") : "'': '") +
               std::string(data) + "'";
    }
};

struct List_Tag : public Tag {
    std::vector<std::unique_ptr<Tag>> data;
    List_Tag(std::optional<std::string_view> name)
        : Tag(tagtype::TAG_List, name) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': List [")
                       : "'': List [");
        for (auto tag_it = data.begin(); tag_it != data.end(); ++tag_it) {
            oss << *tag_it << ", ";
        }
        oss << "]";
        return oss.str();
    }
};

struct Compound_Tag : public Tag {
    std::vector<std::unique_ptr<Tag>> data;
    Compound_Tag(std::optional<std::string_view> name)
        : Tag(tagtype::TAG_Compound, name) {}
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
        : Tag(tagtype::TAG_Int_Array, name), data(data) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': [") : "'': [");
        for (auto element : data) {
            oss << element << ", ";
        }
        oss << "]";
        return oss.str();
    }
};

struct Long_Array_Tag : public Tag {
    std::span<int64_t> data;
    Long_Array_Tag(std::optional<std::string_view> name,
                   std::span<int64_t> data)
        : Tag(tagtype::TAG_Long_Array, name), data(data) {}
    std::string to_string() {
        std::ostringstream oss;
        oss << ((name) ? ("'" + std::string(name.value()) + "': [") : "'': [");
        for (auto element : data) {
            oss << element << "L, ";
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

} // namespace nbtview

#endif // NBTVIEW_H_
