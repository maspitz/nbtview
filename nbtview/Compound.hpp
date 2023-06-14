// Compound.hpp

#ifndef NBT_COMPOUND_H_
#define NBT_COMPOUND_H_

#include <map>
#include <string>
#include <variant>

#include "Compound_fwd.hpp"
#include "Tag.hpp"

namespace nbtview {

class BinaryScanner;
class Tag;

template <typename T> bool Compound::contains(const std::string &name) const {
    auto it = tags.find(name);
    if (it == tags.end()) {
        return false;
    }
    if (!std::holds_alternative<T>(it->second.data)) {
        return false;
    }
    return true;
}

template <typename T> T &Compound::at(const std::string &name) {
    return std::get<T>(tags.at(name).data);
}

} // namespace nbtview

#endif // NBT_COMPOUND_H_
