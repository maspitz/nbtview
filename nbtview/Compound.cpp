// Compound.cpp

#include <map>
#include <variant>

#include "Compound.hpp"
#include "Tag.hpp"
#include "utils.hpp"

namespace nbtview {

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
