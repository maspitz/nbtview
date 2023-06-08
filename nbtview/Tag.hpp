// Tag.hpp

#ifndef NBT_TAG_H_
#define NBT_TAG_H_

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "nbtview.hpp"

namespace nbtview {

using TagData =
    std::variant<Byte, Short, Int, Long, Float, Double,
                 std::unique_ptr<Byte_Array>, String, std::unique_ptr<List>,
                 std::unique_ptr<Compound>, std::unique_ptr<Int_Array>,
                 std::unique_ptr<Long_Array>>;

struct Tag {
    Tag(BinaryScanner &s, TypeCode type);

    TagData data;

    std::string to_string() const;
};

} // namespace nbtview

#endif // NBT_TAG_H_
