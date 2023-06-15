// nbtview.hpp

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <cstdint>
#include <string>
#include <vector>

namespace nbtview {

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

// Attempts to find a named tag in a range of NBT data by searching
// for its initial byte sequence.
//
// If the tag is found, an iterator to the beginning of its payload
// is returned. Otherwise, nbt_stop is returned.
//
// Caveats:
// (1) This method can be fooled if the tag's name appears in the
// payload of some other tag.  This is unlikely to happen by chance.
// (2) It is the calling code's responsibility to check that the
// remaining NBT data is large enough to contain the tag's expected
// payload.

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    TypeCode tag_type, const std::string &tag_name);

} // namespace nbtview

#endif // NBTVIEW_H_
