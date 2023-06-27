// tag_utilities.cpp

#include <string>
#include <variant>
#include <vector>

#include "Tag.hpp"
#include "tag_utilities.hpp"
#include "utils.hpp"

namespace nbtview {

struct StringifyTag {
    std::string operator()(const End &x) { return "END_TAG"; }
    std::string operator()(const Compound &x) {
        std::string output_string;
        for (auto tag_iter = x.cbegin(); tag_iter != x.cend(); ++tag_iter) {
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
            output_string += tag_to_string(tag_iter->second);
        }
        output_string += "}";
        return output_string;
    }
    std::string operator()(const Byte &x) { return std::to_string(x) + "b"; }
    std::string operator()(const Short &x) { return std::to_string(x) + "s"; }
    std::string operator()(const Int &x) { return std::to_string(x); }
    std::string operator()(const Long &x) { return std::to_string(x) + "L"; }
    std::string operator()(const Float &x) { return std::to_string(x) + "f"; }
    std::string operator()(const Double &x) { return std::to_string(x) + "d"; }
    std::string operator()(const Byte_Array &x) {
        return comma_delimited_array(x, array_format{.array_prefix = "[B;",
                                                     .array_suffix = "]",
                                                     .elt_suffix = "b"});
    }
    std::string operator()(const String &x) { return quoted_string(x); }
    std::string operator()(const List &x) {
        std::string output_string;
        for (auto tag_it = x.cbegin(); tag_it != x.cend(); ++tag_it) {
            if (output_string.empty()) {
                output_string = "[";
            } else {
                output_string += ",";
            }
            output_string += tag_to_string(*tag_it);
        }
        output_string += "]";
        return output_string;
    }
    std::string operator()(const Int_Array &x) {
        return comma_delimited_array(x, array_format{.array_prefix = "[I;",
                                                     .array_suffix = "]",
                                                     .elt_suffix = ""});
    }
    std::string operator()(const Long_Array &x) {
        return comma_delimited_array(x, array_format{.array_prefix = "[L;",
                                                     .array_suffix = "]",
                                                     .elt_suffix = "L"});
    }
};

std::string tag_to_string(const Tag &tag) {
    return std::visit(StringifyTag(), tag.data);
}

// std::vector<unsigned char> serialize_tag(const Tag& tag) { }

} // namespace nbtview
