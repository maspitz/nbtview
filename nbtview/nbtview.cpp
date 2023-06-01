#include <algorithm>
#include <bit>
#include <cstdint>
#include <memory>
#include <regex>
#include <span>
#include <type_traits>

#include "Compound.hpp"
#include "List.hpp"
#include "nbtview.hpp"
#include "utils.hpp"

namespace nbtview {

std::unique_ptr<Compound> make_tag_compound(BinaryScanner &s);
std::unique_ptr<List> make_tag_list(BinaryScanner &s);

std::vector<unsigned char>::const_iterator
fast_find_named_tag(std::vector<unsigned char>::const_iterator nbt_start,
                    std::vector<unsigned char>::const_iterator nbt_stop,
                    TypeCode tag_type, const std::string &tag_name) {

    auto loc = nbt_start;
    const char byte_1 = static_cast<char>(tag_type);
    const char byte_2 = tag_name.length() / 16;
    const char byte_3 = tag_name.length() % 16;
    while (1) {
        loc = std::search(loc, nbt_stop, tag_name.begin(), tag_name.end());
        if (loc == nbt_stop) {
            return nbt_stop;
        }
        if (loc - nbt_start >= 3 && *(loc - 3) == byte_1 &&
            *(loc - 2) == byte_2 && *(loc - 1) == byte_3 &&
            tag_name.length() < nbt_stop - loc) {
            return loc + tag_name.length();
        }
        loc++;
    }
}

// TODO: return name of root tag if desired
std::unique_ptr<Compound> make_tag_root(BinaryScanner &s) {
    auto compound_tag = std::make_unique<Compound>();
    auto root_type = static_cast<TypeCode>(s.get_value<int8_t>());
    if (root_type != TypeCode::Compound) {
        throw std::runtime_error("Root tag is not a compound tag");
    }
    auto root_name = s.get_string();
    return make_tag_compound(s);
}

Payload decode_payload(TypeCode type, BinaryScanner &s) {
    switch (type) {
    case TypeCode::End:
        throw std::runtime_error("Unexpected End Tag");
    case TypeCode::Byte:
        return s.get_value<Byte>();
    case TypeCode::Short:
        return s.get_value<Short>();
    case TypeCode::Int:
        return s.get_value<Int>();
    case TypeCode::Long:
        return s.get_value<Long>();
    case TypeCode::Float:
        return s.get_value<Float>();
    case TypeCode::Double:
        return s.get_value<Double>();
    case TypeCode::Byte_Array:
        return s.get_vector<Byte>();
    case TypeCode::String:
        return s.get_string();
    case TypeCode::List:
        return make_tag_list(s);
    case TypeCode::Compound:
        return make_tag_compound(s);
    case TypeCode::Int_Array:
        return s.get_vector<Int>();
    case TypeCode::Long_Array:
        return s.get_vector<Long>();
    default:
        throw std::runtime_error("Unhandled tag type");
    }
    return 0;
}

std::unique_ptr<List> make_tag_list(BinaryScanner &s) {
    auto list_tag = std::make_unique<List>();
    auto list_type = static_cast<TypeCode>(s.get_value<int8_t>());
    auto list_length = s.get_value<int32_t>();
    list_tag->reserve(list_length);
    for (int32_t i = 0; i < list_length; ++i) {
        list_tag->emplace_back(Tag(s, list_type));
    }
    return list_tag;
}

std::unique_ptr<Compound> make_tag_compound(BinaryScanner &s) {
    auto compound_tag = std::make_unique<Compound>();
    auto next_type = static_cast<TypeCode>(s.get_value<int8_t>());
    std::string next_name;
    while (next_type != TypeCode::End) {
        next_name = s.get_string();
        compound_tag->data.emplace(next_name, Tag(s, next_type));
        next_type = static_cast<TypeCode>(s.get_value<int8_t>());
    }
    return compound_tag;
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
    if (std::holds_alternative<Byte>(*this)) {
        return std::to_string(std::get<Byte>(*this)) + "b";
    } else if (std::holds_alternative<Short>(*this)) {
        return std::to_string(std::get<Short>(*this)) + "s";
    } else if (std::holds_alternative<Int>(*this)) {
        return std::to_string(std::get<Int>(*this));
    } else if (std::holds_alternative<Long>(*this)) {
        return std::to_string(std::get<Long>(*this)) + "L";
    } else if (std::holds_alternative<Float>(*this)) {
        return std::to_string(std::get<Float>(*this)) + "f";
    } else if (std::holds_alternative<Double>(*this)) {
        return std::to_string(std::get<Double>(*this)) + "d";
    } else if (std::holds_alternative<std::unique_ptr<Byte_Array>>(*this)) {
        auto &arr = std::get<std::unique_ptr<Byte_Array>>(*this);
        if (arr == nullptr) {
            return "[B;]";
        }
        return comma_delimited_array(*arr, "[B;", "b", "]");
    } else if (std::holds_alternative<String>(*this)) {
        return quoted_string(std::get<String>(*this));
    } else if (std::holds_alternative<std::unique_ptr<List>>(*this)) {
        auto &lst = std::get<std::unique_ptr<List>>(*this);
        if (lst == nullptr) {
            return "[]";
        }
        return lst->to_string();
    } else if (std::holds_alternative<std::unique_ptr<Compound>>(*this)) {
        auto &cpd = std::get<std::unique_ptr<Compound>>(*this);
        if (cpd == nullptr) {
            return "{}";
        }
        return cpd->to_string();
    } else if (std::holds_alternative<std::unique_ptr<Int_Array>>(*this)) {
        auto &arr = std::get<std::unique_ptr<Int_Array>>(*this);
        if (arr == nullptr) {
            return "[I;]";
        }
        return comma_delimited_array(*arr, "[I;", "", "]");
    } else if (std::holds_alternative<std::unique_ptr<Long_Array>>(*this)) {
        auto &arr = std::get<std::unique_ptr<Long_Array>>(*this);
        if (arr == nullptr) {
            return "[L;]";
        }
        return comma_delimited_array(*arr, "[L;", "L", "]");
    } else {
        return "<Unhandled Variant>";
    }
}

} // namespace nbtview
