/**
 * @file Tag.hpp
 * @brief Interfaces for read/write access to NBT tag data
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

//! Utilities to read, write, and manipulate NBT data
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
using Byte_Array_Ptr = std::unique_ptr<Byte_Array>;
//! string of characters (supports UTF-8)
using String = std::string;
using String_Ptr = std::unique_ptr<String>;
//! array of Tag objects
struct List {
    TypeCode tag_type;
    std::vector<Tag> data;
};
using List_Ptr = std::unique_ptr<List>;
//! map from string to Tag objects
using Compound = std::map<std::string, Tag>;
using Compound_Ptr = std::unique_ptr<Compound>;
//! array of Int
using Int_Array = std::vector<Int>;
using Int_Array_Ptr = std::unique_ptr<Int_Array>;
//! array of Long
using Long_Array = std::vector<Long>;
using Long_Array_Ptr = std::unique_ptr<Long_Array>;
//! placeholder type to indicate the absence of a data value
struct None {};

using TagData = std::variant<None, End, Byte, Short, Int, Long, Float, Double,
                             Byte_Array_Ptr, String_Ptr, List_Ptr, Compound_Ptr,
                             Int_Array_Ptr, Long_Array_Ptr>;

/**
 *  @brief Tag holds an NBT tag and provides basic access to it.
 */

class Tag {
  public:
    // Constructors
    Tag() : data(None()) {}
    Tag(End x) : data(x) {}
    Tag(Byte x) : data(x) {}
    Tag(Short x) : data(x) {}
    Tag(Int x) : data(x) {}
    Tag(Long x) : data(x) {}
    Tag(Float x) : data(x) {}
    Tag(Double x) : data(x) {}
    Tag(Byte_Array &&x) : data(std::make_unique<Byte_Array>(std::move(x))) {}
    Tag(const Byte_Array &x) : data(std::make_unique<Byte_Array>(x)) {}
    Tag(String &&x) : data(std::make_unique<String>(std::move(x))) {}
    Tag(const String &x) : data(std::make_unique<String>(x)) {}
    Tag(List &&x) : data(std::make_unique<List>(std::move(x))) {}
    Tag(Compound &&x) : data(std::make_unique<Compound>(std::move(x))) {}
    Tag(Int_Array &&x) : data(std::make_unique<Int_Array>(std::move(x))) {}
    Tag(const Int_Array &x) : data(std::make_unique<Int_Array>(x)) {}
    Tag(Long_Array &&x) : data(std::make_unique<Long_Array>(std::move(x))) {}
    Tag(const Long_Array &x) : data(std::make_unique<Long_Array>(x)) {}

    // TODO consider a function that returns type of an entry.
    /**
     *  @brief Tests whether a Compound Tag contains a particular %Tag
     *
     *  @param name the name of the %Tag
     */
    bool contains(const std::string &name) const {
        if (!std::holds_alternative<Compound_Ptr>(data)) {
            throw std::runtime_error(
                "Can't get named entries from a non-Compound Tag");
        }
        const auto &cmpd = std::get<Compound_Ptr>(data);
        return cmpd->find(name) != cmpd->end();
    }

    // TODO write specializations so that we can get Compound& instead of just
    // Compound_Ptr&
    /**
     * @brief Gets a data entry from a Compound Tag
     *
     * @tparam T the type of data to be accessed.
     * @param name the name of the element to be accessed.
     * @return Reference to the entry's data.
     * @throw std::runtime_error if this is not a Compound Tag.
     * @throw std::out_of_range if no element of that name is present.
     * @throw std::runtime_error if the element of that name is not of type T.
     */
    template <typename T> T &get(const std::string &name) {
        if (!std::holds_alternative<Compound_Ptr>(data)) {
            throw std::runtime_error(
                "Can't get named entries from a non-Compound Tag");
        }
        Tag &tg = std::get<Compound_Ptr>(data)->at(name);
        if (!std::holds_alternative<T>(tg)) {
            throw std::runtime_error("Element " + name +
                                     " is not of the requested type");
        }
        return std::get<T>(tg);
    }

    /* So, here's the design question.
     * Do we want ease of access in navigating a compound tag hierarchy?
     * I.e,  my_region["level"]["structures"]["starts"]    (*)
     * This can be accomplished by using operator[] to return a Tag.
     *
     * Or do we want access more like:
     * my_region.get<Compound>("level").get<Compound>("structures").get<Compound>("starts")
     * (**)
     *
     * (*) is more succinct, but note that we could introduce a separator syntax
     * to achieve similar brevity in a customized accessor:
     *
     * my_region.path_get("/level/structures/starts")
     *
     * Or maybe use '.' as a separator.  Some JSON utilities might do this for
     * nested fields.
     *
     * LET'S MAKE A WISH LIST:
     *
     * 1> Convenient access to nested tags   [soln: path based accessor adapter]
     * 2> Easy access to maybe/optional tags  [soln: provide a std::optional
     * accessor layer?]  [or None tag?] 3>
     *
     */

    /**
     * @brief Returns the size of a container %Tag.
     *
     * @return Number of elements of Tag.
     * @throw  std::runtime_error If Tag is not of a container type: Array,
     * String, List, or Compound.
     */
    std::size_t size() const {
        if (std::holds_alternative<Byte_Array_Ptr>(data)) {
            return std::get<Byte_Array_Ptr>(data)->size();
        } else if (std::holds_alternative<String_Ptr>(data)) {
            return std::get<String_Ptr>(data)->size();
        } else if (std::holds_alternative<List_Ptr>(data)) {
            return std::get<List_Ptr>(data)->data.size();
        } else if (std::holds_alternative<Compound_Ptr>(data)) {
            return std::get<Compound_Ptr>(data)->size();
        } else if (std::holds_alternative<Int_Array_Ptr>(data)) {
            return std::get<Int_Array_Ptr>(data)->size();
        } else if (std::holds_alternative<Long_Array_Ptr>(data)) {
            return std::get<Long_Array_Ptr>(data)->size();
        } else {
            throw std::runtime_error("Called size() on a non-container tag.");
        }
    }

    /**
     * @brief Checks if a Tag of container type is empty.
     *
     * @return True if empty.
     * @throw  std::runtime_error If Tag is not of a container type: Array,
     * String, List, or Compound.
     */
    bool empty() const {
        if (std::holds_alternative<Byte_Array_Ptr>(data)) {
            return std::get<Byte_Array_Ptr>(data)->empty();
        } else if (std::holds_alternative<String_Ptr>(data)) {
            return std::get<String_Ptr>(data)->empty();
        } else if (std::holds_alternative<List_Ptr>(data)) {
            return std::get<List_Ptr>(data)->data.empty();
        } else if (std::holds_alternative<Compound_Ptr>(data)) {
            return std::get<Compound_Ptr>(data)->empty();
        } else if (std::holds_alternative<Int_Array_Ptr>(data)) {
            return std::get<Int_Array_Ptr>(data)->empty();
        } else if (std::holds_alternative<Long_Array_Ptr>(data)) {
            return std::get<Long_Array_Ptr>(data)->empty();
        } else {
            throw std::runtime_error("Called empty() on a non-container tag.");
        }
    }

    // methods for list and array tags
    /**
     * @brief Provides access to the data of a particular entry of a List %Tag
     * or Array %Tag.
     *
     * @tparam T the type of elements in the container: Byte, Int, Long, or Tag.
     * @param index the index of the element to be accessed.
     * @return Read/write reference to the element.
     * @throw std::out_of_range_error If index is out of range.
     * @throw std::runtime_error  If T is of the wrong type or the %Tag is not a
     * container.
     */
    template <typename T> T &get(size_t index) {
        if (std::holds_alternative<std::unique_ptr<std::vector<T>>>(data)) {
            return std::get<std::unique_ptr<std::vector<T>>>(data)->at(index);
        } else {
            throw std::runtime_error("Mismatch in requested tag data type.");
        }
    }

    TagData &tag_data() { return data; }
    const TagData &tag_data() const { return data; }

  private:
    // TagData is a variant type.  Some of its alternatives are unique_ptrs.
    // *** Invariant: We require that `data` cannot be a null pointer. ***
    // This is so that users of Tag::tag_data() may dereference without checking
    // for null.
    TagData data;
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
    auto operator()(const Byte_Array_Ptr &x) { return TypeCode::Byte_Array; }
    auto operator()(const String &x) { return TypeCode::String; }
    auto operator()(const String_Ptr &x) { return TypeCode::String; }
    auto operator()(const List &x) { return TypeCode::List; }
    auto operator()(const List_Ptr &x) { return TypeCode::List; }
    auto operator()(const Compound &x) { return TypeCode::Compound; }
    auto operator()(const Compound_Ptr &x) { return TypeCode::Compound; }
    auto operator()(const Int_Array &x) { return TypeCode::Int_Array; }
    auto operator()(const Int_Array_Ptr &x) { return TypeCode::Int_Array; }
    auto operator()(const Long_Array &x) { return TypeCode::Long_Array; }
    auto operator()(const Long_Array_Ptr &x) { return TypeCode::Long_Array; }
};

} // namespace nbtview

#endif // NBT_TAG_H_
