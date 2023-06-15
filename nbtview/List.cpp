// List.cpp
#include <string>
#include <vector>

#include "BinaryScanner.hpp"
#include "List.hpp"
#include "Tag.hpp"

namespace nbtview {

List::List(BinaryScanner &s) {
    list_type = static_cast<TypeCode>(s.get_value<int8_t>());
    auto list_length = s.get_value<int32_t>();
    tags.reserve(list_length);
    for (int32_t i = 0; i < list_length; ++i) {
        tags.emplace_back(Tag(s, list_type));
    }
}

std::string List::to_string() const {
    std::string output_string;
    for (auto tag_it = tags.cbegin(); tag_it != tags.cend(); ++tag_it) {
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
