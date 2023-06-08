#include <algorithm>
#include <bit>
#include <cstdint>
#include <memory>
#include <regex>
#include <span>
#include <type_traits>

#include "BinaryScanner.hpp"
#include "Compound.hpp"
#include "List.hpp"
#include "nbtview.hpp"
#include "utils.hpp"

namespace nbtview {

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    TypeCode tag_type, const std::string &tag_name) {

    auto loc = nbt_start;
    const char byte_1 = static_cast<char>(tag_type);
    const char byte_2 = tag_name.length() / 16;
    const char byte_3 = tag_name.length() % 16;
    while (1) {
        loc = std::search(loc, nbt_stop, tag_name.begin(), tag_name.end());
        if (loc == nbt_stop) {
            return nbt_stop;
        }
        if (loc - nbt_start >= 3 && *(loc - 3) == byte_1 &&
            *(loc - 2) == byte_2 && *(loc - 1) == byte_3 &&
            tag_name.length() < nbt_stop - loc) {
            return loc + tag_name.length();
        }
        loc++;
    }
}

// TODO: return name of root tag if desired
std::unique_ptr<Compound> make_tag_root(BinaryScanner &s) {
    auto root_type = static_cast<TypeCode>(s.get_value<int8_t>());
    if (root_type != TypeCode::Compound) {
        throw std::runtime_error("Root tag is not a compound tag");
    }
    auto root_name = s.get_string();
    return std::make_unique<Compound>(s);
}

} // namespace nbtview
