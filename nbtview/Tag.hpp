// Tag.hpp

/// \file
/// here is my file

#ifndef NBT_TAG_H_
#define NBT_TAG_H_

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

//! Utilities to read, write, and manipulate NBT data
namespace nbtview {

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
//! array of Int
using Int_Array = std::vector<Int>;
//! array of Long
using Long_Array = std::vector<Long>;
class List;
class Compound;

//! Tag holds the value of an NBT tag.

//! If the NBT tag has a name, it is stored in the Compound tag that contains
//! it. The root tag's name (if it has one) is not stored.
//!
//! You can use the functions provided by <a
//! href="https://en.cppreference.com/w/cpp/header/variant">&lt;variant&gt;</a>
//! to access a Tag's type and value. However, because you usually know what
//! type you expect a given tag to be, it is generally more convenient to get an
//! NBT value of that type via the Compound::get<T> method.
using Tag = std::variant<End, Byte, Short, Int, Long, Float, Double, Byte_Array,
                         String, List, Compound, Int_Array, Long_Array>;

//! TypeCode describes the type of an NBT tag.
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

struct TagID {
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

/*! \brief An NBT Compound tag
 *
 * A Compound object maintains a map with strings for keys and other Tags for
 * values.
 */
class Compound {
  public:
    Compound() : data(std::make_unique<std::map<std::string, Tag>>()) {}

    using iterator = std::map<std::string, Tag>::iterator;
    using const_iterator = std::map<std::string, Tag>::const_iterator;

    auto begin() { return data->begin(); }
    auto end() { return data->end(); }
    auto begin() const { return data->begin(); }
    auto end() const { return data->end(); }
    auto cbegin() const { return data->cbegin(); }
    auto cend() const { return data->cend(); }

    size_t size() const { return data->size(); }
    bool empty() const { return data->empty(); }

    Tag &at(const std::string &key) { return data->at(key); }
    const Tag &at(const std::string &key) const { return data->at(key); }

    // construct element directly in the Compound
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args &&...args) {
        return data->emplace(std::forward<Args>(args)...);
    }

    template <typename T> bool contains(const std::string &name) const;

    template <typename T> void put(const std::string &name, T &&value);

    template <typename T> T &get(const std::string &name);

  private:
    std::unique_ptr<std::map<std::string, Tag>> data;
};

class List {
  public:
    List(TypeCode type)
        : data(std::make_unique<std::vector<Tag>>()), list_type_(type) {}

    auto begin() { return data->begin(); }
    auto end() { return data->end(); }
    auto begin() const { return data->begin(); }
    auto end() const { return data->end(); }
    auto cbegin() const { return data->cbegin(); }
    auto cend() const { return data->cend(); }

    size_t size() const { return data->size(); }
    bool empty() const { return data->empty(); }
    void reserve(size_t new_cap) { data->reserve(new_cap); }

    Tag &at(size_t position) { return data->at(position); }
    const Tag &at(size_t position) const { return data->at(position); }

    // construct element directly in the List
    template <typename... Args> void emplace_back(Args &&...args) {
        data->emplace_back(std::forward<Args>(args)...);
    }

    template <typename T> T &get(size_t pos);

    void push_back(Tag &&t);

    TypeCode list_type() const { return list_type_; }

  private:
    std::unique_ptr<std::vector<Tag>> data;
    TypeCode list_type_;
};

template <typename T> void Compound::put(const std::string &name, T &&value) {
    data->insert_or_assign(name, std::move(value));
}

template <typename T> T &Compound::get(const std::string &name) {
    return std::get<T>(data->at(name));
}

template <typename T> bool Compound::contains(const std::string &name) const {
    auto it = data->find(name);
    if (it == data->end()) {
        return false;
    }
    if (!std::holds_alternative<T>(it->second)) {
        return false;
    }
    return true;
}

template <typename T> T &List::get(size_t pos) {
    return std::get<T>(data->at(pos));
}

} // namespace nbtview

#endif // NBT_TAG_H_
