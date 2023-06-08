// Tag.cpp

#include "Tag.hpp"
#include "BinaryScanner.hpp"
#include "Compound.hpp"
#include "List.hpp"
#include "utils.hpp"
#include <memory>
#include <string>

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
        data = std::make_unique<List>(s);
        break;
    case TypeCode::Compound:
        data = std::make_unique<Compound>(s);
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

template <typename T>
std::string comma_delimited_array(const std::vector<T> &vec,
                                  const std::string &array_prefix,
                                  const std::string &elt_suffix,
                                  const std::string &array_suffix) {
    std::string output_string;
    for (size_t i = 0; i != vec.size(); ++i) {
        if (output_string.empty()) {
            output_string = array_prefix;
        } else {
            output_string += ",";
        }
        output_string += std::to_string(vec[i]) + elt_suffix;
    }
    output_string += array_suffix;
    return output_string;
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
    } else if (std::holds_alternative<std::unique_ptr<Byte_Array>>(data)) {
        auto &arr = std::get<std::unique_ptr<Byte_Array>>(data);
        if (arr == nullptr) {
            return "[B;]";
        }
        return comma_delimited_array(*arr, "[B;", "b", "]");
    } else if (std::holds_alternative<String>(data)) {
        return quoted_string(std::get<String>(data));
    } else if (std::holds_alternative<std::unique_ptr<List>>(data)) {
        auto &lst = std::get<std::unique_ptr<List>>(data);
        if (lst == nullptr) {
            return "[]";
        }
        return lst->to_string();
    } else if (std::holds_alternative<std::unique_ptr<Compound>>(data)) {
        auto &cpd = std::get<std::unique_ptr<Compound>>(data);
        if (cpd == nullptr) {
            return "{}";
        }
        return cpd->to_string();
    } else if (std::holds_alternative<std::unique_ptr<Int_Array>>(data)) {
        auto &arr = std::get<std::unique_ptr<Int_Array>>(data);
        if (arr == nullptr) {
            return "[I;]";
        }
        return comma_delimited_array(*arr, "[I;", "", "]");
    } else if (std::holds_alternative<std::unique_ptr<Long_Array>>(data)) {
        auto &arr = std::get<std::unique_ptr<Long_Array>>(data);
        if (arr == nullptr) {
            return "[L;]";
        }
        return comma_delimited_array(*arr, "[L;", "L", "]");
    } else {
        return "<Unhandled Variant>";
    }
}

} // namespace nbtview
