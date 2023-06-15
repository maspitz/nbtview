// Compound.cpp

#include <map>
#include <string>
#include <variant>

#include "BinaryScanner.hpp"
#include "Compound.hpp"
#include "Tag.hpp"
#include "utils.hpp"

namespace nbtview {

Compound::Compound(BinaryScanner &s) {
    auto get_next_type = [&s]() -> TypeCode {
        return static_cast<TypeCode>(s.get_value<int8_t>());
    };
    for (TypeCode tag_type = get_next_type(); tag_type != TypeCode::End;
         tag_type = get_next_type()) {
        auto tag_name = s.get_string();
        tags.emplace(std::move(tag_name), Tag(s, tag_type));
    }
}

std::string Compound::to_string() const {
    std::string output_string;
    for (auto tag_iter = tags.begin(); tag_iter != tags.end(); ++tag_iter) {
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
