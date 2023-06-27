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

using TagData =
    std::variant<End, Byte, Short, Int, Long, Float, Double, Byte_Array, String,
                 List, Compound, Int_Array, Long_Array>;

struct Tag {
  public:
    TagData data;
    Tag(TagData d) : data(d) {}
};

} // namespace nbtview

#endif // NBT_TAG_H_
