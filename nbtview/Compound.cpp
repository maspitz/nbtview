// Compound.cpp

#include <map>
#include <variant>

#include "BinaryScanner.hpp"
#include "Compound.hpp"
#include "List.hpp"
#include "Tag.hpp"
#include "utils.hpp"

namespace nbtview {

Compound::Compound(BinaryScanner &s) {
    auto next_type = static_cast<TypeCode>(s.get_value<int8_t>());
    std::string next_name;
    while (next_type != TypeCode::End) {
        next_name = s.get_string();
        data.emplace(next_name, Tag(s, next_type));
        next_type = static_cast<TypeCode>(s.get_value<int8_t>());
    }
}

std::string Compound::to_string() const {
    std::string output_string;
    for (auto tag_iter = data.begin(); tag_iter != data.end(); ++tag_iter) {
        if (output_string.empty()) {
            output_string = "{";
        } else {
            output_string += ",";
        }
        if (snbt_requires_quoting(tag_iter->first)) {
            output_string += quoted_string(tag_iter->first);
        } else {
            output_string += tag_iter->first;
        }
        output_string += ":";
        output_string += tag_iter->second.to_string();
    }
    output_string += "}";
    return output_string;
}

} // namespace nbtview
