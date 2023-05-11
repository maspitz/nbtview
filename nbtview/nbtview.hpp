// nbtview.hpp

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace nbtview {

enum class tagtype : char {
    TAG_End = 0,
    TAG_Byte = 1,
    TAG_Short = 2,
    TAG_Int = 3,
    TAG_Long = 4,
    TAG_Float = 5,
    TAG_Double = 6,
    TAG_Byte_Array = 7,
    TAG_String = 8,
    TAG_List = 9,
    TAG_Compound = 10,
    TAG_Int_Array = 11,
    TAG_Long_Array = 12
};

// Attempts to find a named tag in a range of NBT data by searching for its
// initial byte sequence.
//
// If the tag is found, an iterator to the beginning of its payload is returned.
// Otherwise, nbt_stop is returned.
//
// Caveats:
// (1) This method can be fooled if the tag's name appears in the
// payload of some other tag.  This is unlikely to happen by chance.
// (2) It is the calling code's responsibility to check that the remaining NBT
// data is large enough to contain the tag's expected payload.
std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    tagtype tag_type, const std::string &tag_name);

class Tag {
    tagtype type;
};

class Int_Tag : public Tag {
  public:
    int32_t data;
};

class Long_Tag : public Tag {
  public:
    int64_t data;
};

class Compound_Tag : public Tag {
  public:
    std::vector<std::unique_ptr<Tag>> data;
};

// emplace_tag reads the tag which begins at input, emplaces the constructed tag
// at the output, and returns an iterator advanced just past the tag that was
// read.
template <typename InputIterator, typename OutputIterator>
InputIterator emplace_tag(InputIterator input, OutputIterator output);

} // namespace nbtview

#endif // NBTVIEW_H_
