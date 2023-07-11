// BinaryDeserializer.cpp

#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "Compound_fwd.hpp"
#include "List.hpp"
#include "Tag.hpp"

#include "BinaryDeserializer.hpp"
#include "BinaryScanner.hpp"

namespace nbtview {

BinaryDeserializer::BinaryDeserializer(std::vector<unsigned char> bytes)
    : scanner(std::make_unique<BinaryScanner>(std::move(bytes))) {}

BinaryDeserializer::~BinaryDeserializer() = default;

// deserialize reads the tag type, tag name, and tag payload, and returns the
// tag's name and value.
std::pair<std::string, Tag> BinaryDeserializer::deserialize() {
    // read type id byte
    TypeCode type = static_cast<TypeCode>(scanner->get_value<int8_t>());
    if (type == TypeCode::End) {
        return std::make_pair("", Tag(End()));
    }
    std::string tag_name = scanner->get_string();
    return std::make_pair(tag_name, deserialize_typed_value(type));
}

List BinaryDeserializer::deserialize_list() {
    auto list_type = static_cast<TypeCode>(scanner->get_value<int8_t>());
    auto list_length = scanner->get_value<int32_t>();
    auto lst = List(list_type);
    lst.reserve(list_length);
    for (int32_t idx = 0; idx < list_length; ++idx) {
        auto next_tag = deserialize_typed_value(list_type);
        lst.emplace_back(std::move(next_tag));
    }
    return lst;
}

Compound BinaryDeserializer::deserialize_compound() {
    auto cmpd = Compound();
    while (true) {
        auto next_tag = deserialize();
        if (std::holds_alternative<End>(next_tag.second)) {
            return cmpd;
        }
        cmpd.emplace(std::move(next_tag.first), std::move(next_tag.second));
    }
}

Tag BinaryDeserializer::deserialize_typed_value(TypeCode type) {
    switch (type) {
    case TypeCode::Byte:
        return scanner->get_value<Byte>();
    case TypeCode::Short:
        return scanner->get_value<Short>();
    case TypeCode::Int:
        return scanner->get_value<Int>();
    case TypeCode::Long:
        return scanner->get_value<Long>();
    case TypeCode::Float:
        return scanner->get_value<Float>();
    case TypeCode::Double:
        return scanner->get_value<Double>();
    case TypeCode::Byte_Array:
        return scanner->get_vector<Byte>();
    case TypeCode::String:
        return scanner->get_string();
    case TypeCode::List:
        return deserialize_list();
    case TypeCode::Compound:
        return deserialize_compound();
    case TypeCode::Int_Array:
        return scanner->get_vector<Int>();
    case TypeCode::Long_Array:
        return scanner->get_vector<Long>();
    default:
        throw std::runtime_error("Unhandled tag type");
    }
}

} // namespace nbtview
