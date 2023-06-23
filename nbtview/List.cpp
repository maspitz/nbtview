// List.cpp
#include <string>
#include <vector>

#include "List.hpp"
#include "Tag.hpp"

namespace nbtview {

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

List::size_type List::size() { return tags.size(); }

} // namespace nbtview
