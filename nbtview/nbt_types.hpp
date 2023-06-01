// nbt_types.hpp

#ifndef NBT_TYPES_H_
#define NBT_TYPES_H_

#include <cstdint>
#include <memory>
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

} // namespace nbtview

#endif // NBT_TYPES_H_
