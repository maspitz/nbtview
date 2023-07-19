// Tag.cpp

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "Stringifier.hpp"
#include "Tag.hpp"

namespace nbtview {

void List::push_back(Tag &&t) {
    if (std::visit(TagID(), t) != list_type_) {
        auto error_str = std::string("Type mismatch when appending ") +
                         tag_type_to_string(t) + " " + tag_to_string(t) +
                         " to " + tag_type_to_string(*this) + " " +
                         tag_to_string(*this);
        throw std::runtime_error(error_str);
    }
    data->push_back(std::move(t));
}

} // namespace nbtview
