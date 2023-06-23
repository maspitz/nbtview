// Tag.cpp

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "Compound.hpp"
#include "List.hpp"
#include "Tag.hpp"
#include "utils.hpp"

namespace nbtview {

std::string Tag::to_string() const {
    if (std::holds_alternative<Byte>(data)) {
        return std::to_string(std::get<Byte>(data)) + "b";
    } else if (std::holds_alternative<Short>(data)) {
        return std::to_string(std::get<Short>(data)) + "s";
    } else if (std::holds_alternative<Int>(data)) {
        return std::to_string(std::get<Int>(data));
    } else if (std::holds_alternative<Long>(data)) {
        return std::to_string(std::get<Long>(data)) + "L";
    } else if (std::holds_alternative<Float>(data)) {
        return std::to_string(std::get<Float>(data)) + "f";
    } else if (std::holds_alternative<Double>(data)) {
        return std::to_string(std::get<Double>(data)) + "d";
    } else if (std::holds_alternative<Byte_Array>(data)) {
        auto arr = std::get<Byte_Array>(data);
        return comma_delimited_array(arr, array_format{.array_prefix = "[B;",
                                                       .array_suffix = "]",
                                                       .elt_suffix = "b"});
    } else if (std::holds_alternative<String>(data)) {
        return quoted_string(std::get<String>(data));
    } else if (std::holds_alternative<List>(data)) {
        auto lst = std::get<List>(data);
        return lst.to_string();
    } else if (std::holds_alternative<Compound>(data)) {
        auto cpd = std::get<Compound>(data);
        return cpd.to_string();
    } else if (std::holds_alternative<Int_Array>(data)) {
        auto arr = std::get<Int_Array>(data);
        return comma_delimited_array(arr, array_format{.array_prefix = "[I;",
                                                       .array_suffix = "]",
                                                       .elt_suffix = ""});
    } else if (std::holds_alternative<Long_Array>(data)) {
        auto &arr = std::get<Long_Array>(data);
        return comma_delimited_array(arr, array_format{.array_prefix = "[L;",
                                                       .array_suffix = "]",
                                                       .elt_suffix = "L"});
    } else {
        return "<Unhandled Variant>";
    }
}

} // namespace nbtview
