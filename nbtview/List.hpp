// List.hpp
#ifndef NBT_LIST_H_
#define NBT_LIST_H_

#include <variant>

#include "List_fwd.hpp"
#include "Tag.hpp"

namespace nbtview {

template <typename T> T &List::at(List::size_type pos) {
    return std::get<T>(tags.at(pos).data);
}

// Any templated methods on List that must appear in a header file should go
// here, just like in Compound.hpp.  (There aren't any yet.)

} // namespace nbtview

#endif // NBT_LIST_H_
