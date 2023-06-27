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

} // namespace nbtview

#endif // NBT_TAG_H_
