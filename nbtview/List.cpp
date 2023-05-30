// List.cpp
#include "List.hpp"
#include "nbtview.hpp" // for Tag interface

namespace nbtview {

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
