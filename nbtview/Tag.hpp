// Tag.hpp

#ifndef NBT_TAG_H_
#define NBT_TAG_H_

#include <variant>

#include "nbtview.hpp"

namespace nbtview {

class List;
class Compound;

using Tag = std::variant<End, Byte, Short, Int, Long, Float, Double, Byte_Array,
                         String, List, Compound, Int_Array, Long_Array>;

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
