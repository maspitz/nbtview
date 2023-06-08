// List.cpp
#include <string>

#include "BinaryScanner.hpp"
#include "Compound.hpp"
#include "List.hpp"

namespace nbtview {

List::List(BinaryScanner &s) : std::vector<Tag>() {
    list_type = static_cast<TypeCode>(s.get_value<int8_t>());
    auto list_length = s.get_value<int32_t>();
    this->reserve(list_length);
    for (int32_t i = 0; i < list_length; ++i) {
        this->emplace_back(Tag(s, list_type));
    }
}

std::string List::to_string() {
    std::string output_string;
    for (auto tag_it = this->begin(); tag_it != this->end(); ++tag_it) {
        if (output_string.empty()) {
            output_string = "[";
        } else {
            output_string += ",";
        }
        output_string += tag_it->to_string();
    }
    output_string += "]";
    return output_string;
}

} // namespace nbtview
