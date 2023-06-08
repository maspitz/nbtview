// nbtview.hpp

#ifndef NBTVIEW_H_
#define NBTVIEW_H_

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "Tag.hpp"

#include "BinaryScanner.hpp"
#include "Compound.hpp"
#include "List.hpp"

namespace nbtview {

std::unique_ptr<Compound> make_tag_root(BinaryScanner &s);

// Attempts to find a named tag in a range of NBT data by searching for
// its initial byte sequence.
//
// If the tag is found, an iterator to the beginning of its payload is
// returned. Otherwise, nbt_stop is returned.
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
