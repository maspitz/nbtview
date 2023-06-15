// Tag.hpp

#ifndef NBT_TAG_H_
#define NBT_TAG_H_

#include <string>
#include <variant>

#include "Compound_fwd.hpp"
#include "List_fwd.hpp"
#include "nbtview.hpp"

namespace nbtview {

class BinaryScanner;

using TagData = std::variant<Byte, Short, Int, Long, Float, Double, Byte_Array,
                             String, List, Compound, Int_Array, Long_Array>;

class Tag {
  public:
    Tag(BinaryScanner &s, TypeCode type);

    TagData data;

    std::string to_string() const;
};

} // namespace nbtview

#endif // NBT_TAG_H_
