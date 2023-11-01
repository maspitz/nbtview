/**
 * @file Tag.hpp
 * @brief Stores and provides access to NBT tag data
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef NBT_TAG_H_
#define NBT_TAG_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "utils.hpp"

namespace nbtview {

class Tag;

//! TypeCode is a one-byte encoding of the data type of an NBT tag.
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
    Long_Array = 12,
    None = 99
};

const char *typecode_string(TypeCode type) {
    switch (type) {
    case TypeCode::End:
        return "End";
    case TypeCode::Byte:
        return "Byte";
    case TypeCode::Short:
        return "Short";
    case TypeCode::Int:
        return "Int";
    case TypeCode::Long:
        return "Long";
    case TypeCode::Float:
        return "Float";
    case TypeCode::Double:
        return "Double";
    case TypeCode::Byte_Array:
        return "Byte_Array";
    case TypeCode::String:
        return "String";
    case TypeCode::List:
        return "List";
    case TypeCode::Compound:
        return "Compound";
    case TypeCode::Int_Array:
        return "Int_Array";
    case TypeCode::Long_Array:
        return "Long_Array";
    case TypeCode::None:
        return "None";
    default:
        return "Invalid TypeCode";
    }
}

//! placeholder type to indicate the absence of a data value
struct None {};
//! NBT End tag: marks the end of a Compound Tag's payload
using End = unsigned char;
//! 8-bit signed integer
using Byte = int8_t;
//! 16-bit signed integer
using Short = int16_t;
//! 32-bit signed integer
using Int = int32_t;
//! 64-bit signed integer
using Long = int64_t;
//! 32-bit floating point data
using Float = float;
//! 64-bit floating point data
using Double = double;
//! array of Byte
using Byte_Array = std::vector<Byte>;
//! string of characters (supports UTF-8)
using String = std::string;
//! array of Tag objects
class List;
//! map from string to Tag objects
class Compound;
//! array of Int
using Int_Array = std::vector<Int>;
//! array of Long
using Long_Array = std::vector<Long>;

class List {
  private:
    std::vector<Tag> data;

  public:
    // inline void push_back(const Tag &value);
    // inline Tag &operator[](size_t index);
    void push_back(const Tag &value) { data.push_back(value); }
    Tag &operator[](size_t index) { return data[index]; }
    const Tag &operator[](size_t index) const { return data[index]; }
    auto begin() { return data.begin(); }
    const auto begin() const { return data.begin(); }
    auto end() { return data.end(); }
    const auto end() const { return data.end(); }
    const std::size_t size() const { return data.size(); }
    const bool empty() const { return data.empty(); }
    const TypeCode list_type() const;
};

class Compound {
  private:
    std::map<std::string, Tag> data;

  public:
    inline void insert(const std::string &key, const Tag &value);
    Tag &operator[](const std::string &key) { return data[key]; }
    Tag &operator[](std::string &&key) { return data[std::move(key)]; }
    bool contains(const std::string &key) const;

    auto begin() { return data.begin(); }
    const auto begin() const { return data.begin(); }
    auto end() { return data.end(); }
    const auto end() const { return data.end(); }
    const std::size_t size() const { return data.size(); }
    const bool empty() const { return data.empty(); }
};

//! TagID is used with std::visit to get the TypeCode of a given Tag
struct TagID {
    auto operator()(const None &x) { return TypeCode::None; }
    auto operator()(const End &x) { return TypeCode::End; }
    auto operator()(const Byte &x) { return TypeCode::Byte; }
    auto operator()(const Short &x) { return TypeCode::Short; }
    auto operator()(const Int &x) { return TypeCode::Int; }
    auto operator()(const Long &x) { return TypeCode::Long; }
    auto operator()(const Float &x) { return TypeCode::Float; }
    auto operator()(const Double &x) { return TypeCode::Double; }
    auto operator()(const Byte_Array &x) { return TypeCode::Byte_Array; }
    auto operator()(const String &x) { return TypeCode::String; }
    auto operator()(const List &x) { return TypeCode::List; }
    auto operator()(const Compound &x) { return TypeCode::Compound; }
    auto operator()(const Int_Array &x) { return TypeCode::Int_Array; }
    auto operator()(const Long_Array &x) { return TypeCode::Long_Array; }
};

class Tag {
  public:
    using Value =
        std::variant<None, End, Byte, Short, Int, Long, Float, Double,
                     Byte_Array, String, List, Compound, Int_Array, Long_Array>;

  private:
    Value value;

  public:
    template <typename T> Tag(const T &v) : value(v) {}
    Tag() : value(None{}) {}

    template <typename T> T &get() { return std::get<T>(value); }

    template <typename T> const T &get() const { return std::get<T>(value); }

    Value &get_value() { return value; }
    const Value &get_value() const { return value; }

    const TypeCode get_id() const { return std::visit(TagID(), value); }

    template <typename T> bool is() const {
        return std::holds_alternative<T>(value);
    }
    const std::size_t size() const {
        if (std::holds_alternative<Compound>(value)) {
            return std::get<Compound>(value).size();
        } else if (std::holds_alternative<List>(value)) {
            return std::get<List>(value).size();
        }
        throw std::runtime_error(
            "Called size() on a Tag which is neither Compound nor List.");
        return 0;
    }
    bool empty() const {
        if (std::holds_alternative<Compound>(value)) {
            return std::get<Compound>(value).empty();
        } else if (std::holds_alternative<List>(value)) {
            return std::get<List>(value).empty();
        }
        throw std::runtime_error(
            "Called empty() on a Tag which is neither Compound nor List.");
        return 0;
    }

    /**
     *  @brief Tests whether a Compound Tag contains a particular %Tag
     *
     *  @param key the name of the %Tag being queried for
     */
    bool contains(const std::string &key) const {
        return std::get<Compound>(value).contains(key);
    }
    Tag &operator[](const std::string &key) {
        return std::get<Compound>(value)[key];
    }
    Tag &operator[](size_t index) { return std::get<List>(value)[index]; }
    const Tag &operator[](size_t index) const {
        return std::get<List>(value)[index];
    }
};

const TypeCode List::list_type() const {
    if (empty()) {
        return TypeCode::End;
    } else {
        return data[0].get_id();
    }
}

void Compound::insert(const std::string &key, const Tag &value) {
    data[key] = value;
}

bool Compound::contains(const std::string &key) const {
    return data.find(key) != data.end();
}

std::string to_string(const Tag &tag);

const char *typecode_to_string(TypeCode type) {
    switch (type) {
    case TypeCode::End:
        return "End";
    case TypeCode::Byte:
        return "Byte";
    case TypeCode::Short:
        return "Short";
    case TypeCode::Int:
        return "Int";
    case TypeCode::Long:
        return "Long";
    case TypeCode::Float:
        return "Float";
    case TypeCode::Double:
        return "Double";
    case TypeCode::Byte_Array:
        return "Byte_Array";
    case TypeCode::String:
        return "String";
    case TypeCode::List:
        return "List";
    case TypeCode::Compound:
        return "Compound";
    case TypeCode::Int_Array:
        return "Int_Array";
    case TypeCode::Long_Array:
        return "Long_Array";
    case TypeCode::None:
        return "None";
    default:
        return "Invalid TypeCode";
    }
}

std::string tag_id_string(const Tag &tag) {
    auto id = tag.get_id();
    if (id == TypeCode::List) {
        return tag.empty() ? "List of End"
                           : std::string("List of ") +
                                 typecode_to_string(tag[0].get_id());
    } else {
        return typecode_to_string(id);
    }
}

template <typename T> std::string associative_to_string(const T &iterable) {
    std::string output_string;
    for (const auto &elt : iterable) {
        if (output_string.empty()) {
            output_string = "{";
        } else {
            output_string += ",";
        }
        if (snbt_requires_quoting(elt.first)) {
            output_string += quoted_string(elt.first);
        } else {
            output_string += elt.first;
        }

        output_string += ":" + to_string(elt.second);
    }
    output_string += "}";
    return output_string;
}

template <typename T>
std::string sequential_to_string(const T &iterable,
                                 const std::string &array_prefix) {
    std::string output_string;
    for (const auto &elt : iterable) {
        if (output_string.empty()) {
            output_string = "[" + array_prefix;
        } else {
            output_string += ",";
        }
        output_string += to_string(elt);
    }
    output_string += "]";
    return output_string;
}

template <typename T>
std::string array_to_string(const T &iterable, const std::string &array_prefix,
                            const std::string &element_suffix) {
    std::string output_string;
    for (const auto &elt : iterable) {
        if (output_string.empty()) {
            output_string = "[" + array_prefix;
        } else {
            output_string += ",";
        }
        output_string += std::to_string(elt) + element_suffix;
    }
    output_string += "]";
    return output_string;
}

std::string to_string(const Tag &tag) {
    struct ToStringVisitor {
        std::string operator()(const None x) const { return "<<NONE>>"; }
        std::string operator()(const End x) const { return "<<END_TAG>>"; }
        std::string operator()(const Compound &x) const {
            return associative_to_string(x);
        }
        std::string operator()(const Byte x) const {
            return std::to_string(x) + "b";
        }
        std::string operator()(const Short x) const {
            return std::to_string(x) + "s";
        }
        std::string operator()(const Int x) const { return std::to_string(x); }
        std::string operator()(const Long x) const {
            return std::to_string(x) + "L";
        }
        std::string operator()(const Float x) const {
            return std::to_string(x) + "f";
        }
        std::string operator()(const Double x) const {
            return std::to_string(x) + "d";
        }
        std::string operator()(const Byte_Array &x) const {
            return array_to_string(x, "B;", "b");
        }
        std::string operator()(const String &x) const {
            return quoted_string(x);
        }
        std::string operator()(const List &x) const {
            return sequential_to_string(x, "");
        }
        std::string operator()(const Int_Array &x) const {
            return array_to_string(x, "I;", "");
        }
        std::string operator()(const Long_Array &x) const {
            return array_to_string(x, "L;", "L");
        }
    };

    return std::visit(ToStringVisitor{}, tag.get_value());
}

std::ostream &operator<<(std::ostream &os, const Tag &tag) {
    return (os << to_string(tag));
}

} // namespace nbtview

#endif // NBT_TAG_H_
