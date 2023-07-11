// Compound.hpp

#ifndef NBT_COMPOUND_H_
#define NBT_COMPOUND_H_

#include <map>
#include <string>
#include <variant>

#include "Compound_fwd.hpp"
#include "List.hpp"
#include "Tag.hpp"

namespace nbtview {

template <typename T> bool Compound::contains(const std::string &name) const {
    auto it = data->find(name);
    if (it == data->end()) {
        return false;
    }
    if (!std::holds_alternative<T>(it->second)) {
        return false;
    }
    return true;
}

template <typename T> void Compound::put(const std::string &name, T &&value) {
    data->insert_or_assign(name, std::move(value));
}

template <typename T> T &Compound::get(const std::string &name) {
    return std::get<T>(data->at(name));
}

} // namespace nbtview

#endif // NBT_COMPOUND_H_
