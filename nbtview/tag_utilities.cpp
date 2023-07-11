// tag_utilities.cpp

#include <string>
#include <variant>
#include <vector>

#include "Compound_fwd.hpp"
#include "List.hpp"
#include "Tag.hpp"
#include "tag_utilities.hpp"
#include "utils.hpp"

namespace nbtview {

struct StringifyTag {
    std::string operator()(const End &x) { return "END_TAG"; }
    std::string operator()(const Compound &x) {
        std::string output_string;
        for (const auto &elt : x) {
            const std::string &elt_name(elt.first);
            const Tag &elt_value(elt.second);
            if (output_string.empty()) {
                output_string = "{";
            } else {
                output_string += ",";
            }
            if (snbt_requires_quoting(elt_name)) {
                output_string += quoted_string(elt_name);
            } else {
                output_string += elt_name;
            }
            output_string += ":";
            output_string += tag_to_string(elt_value);
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
        for (const auto &elt : x) {
            if (output_string.empty()) {
                output_string = "[";
            } else {
                output_string += ",";
            }
            output_string += tag_to_string(elt);
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
    return std::visit(StringifyTag(), tag);
}

// tag_to_string(const Tag &tag) cannot be invoked
// on a non-const List or Compound because the implicit conversion to a const
// Tag& would require copy construction of a List or Compound, which we have
// intentionally left deleted (due to their noncopyable member data).  Instead,
// simply allow override resolution to handle those situations:

std::string tag_to_string(const List &tag) { return StringifyTag{}(tag); }
std::string tag_to_string(const Compound &tag) { return StringifyTag{}(tag); }

struct StringifyTagType {
    std::string operator()(const End &x) { return "End"; }
    std::string operator()(const Compound &x) { return "Compound"; }
    std::string operator()(const Byte &x) { return "Byte"; }
    std::string operator()(const Short &x) { return "Short"; }
    std::string operator()(const Int &x) { return "Int"; }
    std::string operator()(const Long &x) { return "Long"; }
    std::string operator()(const Float &x) { return "Float"; }
    std::string operator()(const Double &x) { return "Double"; }
    std::string operator()(const Byte_Array &x) { return "Byte_Array"; }
    std::string operator()(const String &x) { return "String"; }
    std::string operator()(const List &x) {
        return std::string("List of ") + tag_code_to_string(x.list_type());
    }
    std::string operator()(const Int_Array &x) { return "Int_Array"; }
    std::string operator()(const Long_Array &x) { return "Long_Array"; }
};

std::string tag_type_to_string(const Tag &tag) {
    return std::visit(StringifyTagType(), tag);
}

std::string tag_type_to_string(const List &tag) {
    return StringifyTagType{}(tag);
}
std::string tag_type_to_string(const Compound &tag) {
    return StringifyTagType{}(tag);
}

std::string tag_code_to_string(TypeCode type) {
    switch (type) {
    case TypeCode::End:
        return "End";
    case TypeCode::Byte:
        return "Byte";
    case TypeCode::Short:
        return "Short";
    case TypeCode::Int:
        return "Int";
    case TypeCode::Long:
        return "Long";
    case TypeCode::Float:
        return "Float";
    case TypeCode::Double:
        return "Double";
    case TypeCode::Byte_Array:
        return "Byte_Array";
    case TypeCode::String:
        return "String";
    case TypeCode::List:
        return "List";
    case TypeCode::Compound:
        return "Compound";
    case TypeCode::Int_Array:
        return "Int_Array";
    case TypeCode::Long_Array:
        return "Long_Array";
    default:
        return "Unknown Tag Type";
    }
}

} // namespace nbtview
