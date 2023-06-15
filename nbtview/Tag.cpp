// Tag.cpp

#include "Tag.hpp"
#include "BinaryScanner.hpp"
#include "Compound.hpp"
#include "List.hpp"
#include "utils.hpp"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace nbtview {

Tag::Tag(BinaryScanner &s, TypeCode type) {
    switch (type) {
    case TypeCode::End:
        throw std::runtime_error("Unexpected End Tag");
    case TypeCode::Byte:
        data = s.get_value<Byte>();
        break;
    case TypeCode::Short:
        data = s.get_value<Short>();
        break;
    case TypeCode::Int:
        data = s.get_value<Int>();
        break;
    case TypeCode::Long:
        data = s.get_value<Long>();
        break;
    case TypeCode::Float:
        data = s.get_value<Float>();
        break;
    case TypeCode::Double:
        data = s.get_value<Double>();
        break;
    case TypeCode::Byte_Array:
        data = s.get_vector<Byte>();
        break;
    case TypeCode::String:
        data = s.get_string();
        break;
    case TypeCode::List:
        data = List(s);
        break;
    case TypeCode::Compound:
        data = Compound(s);
        break;
    case TypeCode::Int_Array:
        data = s.get_vector<Int>();
        break;
    case TypeCode::Long_Array:
        data = s.get_vector<Long>();
        break;
    default:
        throw std::runtime_error("Unhandled tag type");
    }
}

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
        return comma_delimited_array(arr, "[B;", "b", "]");
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
        return comma_delimited_array(arr, "[I;", "", "]");
    } else if (std::holds_alternative<Long_Array>(data)) {
        auto &arr = std::get<Long_Array>(data);
        return comma_delimited_array(arr, "[L;", "L", "]");
    } else {
        return "<Unhandled Variant>";
    }
}

} // namespace nbtview
