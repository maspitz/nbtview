// Tag.hpp

#ifndef NBT_TAG_H_
#define NBT_TAG_H_

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace nbtview {

using End = unsigned char;
using Byte = int8_t;
using Short = int16_t;
using Int = int32_t;
using Long = int64_t;
using Float = float;
using Double = double;
using Byte_Array = std::vector<Byte>;
using String = std::string;
using Int_Array = std::vector<Int>;
using Long_Array = std::vector<Long>;

class List;
class Compound;

using Tag = std::variant<End, Byte, Short, Int, Long, Float, Double, Byte_Array,
                         String, List, Compound, Int_Array, Long_Array>;

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

} // namespace nbtview

#endif // NBT_TAG_H_
