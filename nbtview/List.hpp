// List.hpp
#ifndef NBT_LIST_H_
#define NBT_LIST_H_

#include <variant>

#include "Compound_fwd.hpp"
#include "List_fwd.hpp"
#include "Tag.hpp"

namespace nbtview {

template <typename T> T &List::get(size_t pos) {
    return std::get<T>(data->at(pos));
}

// void List::reserve(size_t new_cap) { data->reserve(new_cap); }

} // namespace nbtview

#endif // NBT_LIST_H_
