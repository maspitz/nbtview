// nbtview.hpp

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "BinaryScanner.hpp"

namespace nbtview {

using Byte = int8_t;
using Short = int16_t;
using Int = int32_t;
using Long = int64_t;
using Float = float;
using Double = double;
using Byte_Array = std::vector<Byte>;
using String = std::string;
class List;
class Compound;
using Int_Array = std::vector<Int>;
using Long_Array = std::vector<Long>;

enum class TypeCode : char {
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
    Long_Array = 12
};

std::unique_ptr<Compound> make_tag_root(BinaryScanner &s);

using Payload =
    std::variant<Byte, Short, Int, Long, Float, Double,
                 std::unique_ptr<Byte_Array>, String, std::unique_ptr<List>,
                 std::unique_ptr<Compound>, std::unique_ptr<Int_Array>,
                 std::unique_ptr<Long_Array>>;

Payload decode_payload(TypeCode type, BinaryScanner &s);

struct Tag : public Payload {
    Tag(BinaryScanner &s, TypeCode type) : Payload(decode_payload(type, s)) {}

    std::string to_string() const;
};

// Attempts to find a named tag in a range of NBT data by searching for
// its initial byte sequence.
//
// If the tag is found, an iterator to the beginning of its payload is
// returned. Otherwise, nbt_stop is returned.
//
// Caveats:
// (1) This method can be fooled if the tag's name appears in the
// payload of some other tag.  This is unlikely to happen by chance.
// (2) It is the calling code's responsibility to check that the
// remaining NBT data is large enough to contain the tag's expected
// payload.

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    TypeCode tag_type, const std::string &tag_name);

struct List : public std::vector<Tag> {
  public:
    using base = std::vector<Tag>;

    // inherit constructors from std::vector
    using base::base;

    std::string to_string() { return "LIST"; }
};

struct Compound {

  public:
    std::map<std::string, Tag> data;
    Compound() {}

    std::optional<Byte> get_Byte(const std::string &name) const {
        return get_value<Byte>(name);
    }

    std::optional<Short> get_Short(const std::string &name) const {
        return get_value<Short>(name);
    }

    std::optional<Int> get_Int(const std::string &name) const {
        return get_value<Int>(name);
    }

    std::optional<Long> get_Long(const std::string &name) const {
        return get_value<Long>(name);
    }

    std::optional<Float> get_Float(const std::string &name) const {
        return get_value<Float>(name);
    }

    std::optional<Double> get_Double(const std::string &name) const {
        return get_value<Double>(name);
    }

    const Byte_Array *get_Byte_Array(const std::string &name) const {
        return get_ptr<Byte_Array>(name);
    }

    std::optional<String> get_String(const std::string &name) const {
        return get_value<String>(name);
    }
    const List *get_List(const std::string &name) const {
        return get_ptr<List>(name);
    }

    const Compound *get_Compound(const std::string &name) const {
        return get_ptr<Compound>(name);
    }

    const Int_Array *get_Int_Array(const std::string &name) const {
        return get_ptr<Int_Array>(name);
    }

    const Long_Array *get_Long_Array(const std::string &name) const {
        return get_ptr<Long_Array>(name);
    }

    std::string to_string() {
        std::ostringstream oss;
        oss << "Compound [";
        for (auto tag_it = data.begin(); tag_it != data.end(); ++tag_it) {
            oss << tag_it->second.to_string() << ", ";
        }
        oss << "]";
        return oss.str();
    }

  private:
    template <typename T>
    std::optional<T> get_value(const std::string &name) const {
        auto it = data.find(name);
        if (it == data.end()) {
            return std::nullopt;
        }
        if (!std::holds_alternative<T>(it->second)) {
            return std::nullopt;
        }
        return std::get<T>(it->second);
    }

    template <typename T> const T *get_ptr(const std::string &name) const {
        auto it = data.find(name);
        if (it == data.end()) {
            return nullptr;
        }
        if (!std::holds_alternative<std::unique_ptr<T>>(it->second)) {
            return nullptr;
        }
        return std::get<std::unique_ptr<T>>(it->second).get();
    }
};

inline std::string Tag::to_string() const {
    if (std::holds_alternative<Byte>(*this)) {
        return std::to_string(std::get<Byte>(*this)) + "b";
    } else if (std::holds_alternative<Short>(*this)) {
        return std::to_string(std::get<Short>(*this)) + "s";
    } else if (std::holds_alternative<Int>(*this)) {
        return std::to_string(std::get<Int>(*this));
    } else if (std::holds_alternative<Long>(*this)) {
        return std::to_string(std::get<Long>(*this)) + "L";
    } else if (std::holds_alternative<Float>(*this)) {
        return std::to_string(std::get<Float>(*this)) + "f";
    } else if (std::holds_alternative<Double>(*this)) {
        return std::to_string(std::get<Double>(*this)) + "d";
    } else if (std::holds_alternative<std::unique_ptr<Byte_Array>>(*this)) {
        auto &arr = std::get<std::unique_ptr<Byte_Array>>(*this);
        if (arr == nullptr) {
            return "[null byte array]";
        }
        return "[" + std::to_string(arr->size()) + " bytes]";
    } else if (std::holds_alternative<String>(*this)) {
        return "'" + std::get<String>(*this) + "''";
    } else if (std::holds_alternative<std::unique_ptr<List>>(*this)) {
        auto &lst = std::get<std::unique_ptr<List>>(*this);
        if (lst == nullptr) {
            return "[null list]";
        }
        return lst->to_string();
    } else if (std::holds_alternative<std::unique_ptr<Compound>>(*this)) {
        auto &cpd = std::get<std::unique_ptr<Compound>>(*this);
        if (cpd == nullptr) {
            return "[null compound]";
        }
        return cpd->to_string();
    } else if (std::holds_alternative<std::unique_ptr<Int_Array>>(*this)) {
        auto &arr = std::get<std::unique_ptr<Int_Array>>(*this);
        if (arr == nullptr) {
            return "[null int array]";
        }
        return "[" + std::to_string(arr->size()) + " ints]";
    } else if (std::holds_alternative<std::unique_ptr<Long_Array>>(*this)) {
        auto &arr = std::get<std::unique_ptr<Long_Array>>(*this);
        if (arr == nullptr) {
            return "[null long array]";
        }
        return "[" + std::to_string(arr->size()) + " longs]";
    } else {
        return "<Unhandled Variant>";
    }
}

} // namespace nbtview

#endif // NBTVIEW_H_
