// BinaryDeserializer.cpp

#include <utility>
#include <variant>
#include <vector>

#include "Tag.hpp"

#include "BinaryDeserializer.hpp"
#include "BinaryReader.hpp"

namespace nbtview {

// deserialize reads the tag type, tag name, and tag payload, and returns the
// tag's name and value.
std::pair<std::string, Tag> BinaryDeserializer::deserialize() {
    // read type id byte
    TypeCode type = static_cast<TypeCode>(scanner.read<int8_t>());
    if (type == TypeCode::End) {
        return {"", Tag(End())};
    }
    std::string tag_name = deserialize_string();
    return std::make_pair(tag_name, deserialize_typed_value(type));
}

List BinaryDeserializer::deserialize_list() {
    auto list_type = static_cast<TypeCode>(scanner.read<int8_t>());
    auto list_length = scanner.read<int32_t>();
    List lst;
    lst.reserve(list_length);
    for (int32_t idx = 0; idx < list_length; ++idx) {
        auto next_tag_data = deserialize_typed_value(list_type);
        lst.emplace_back(std::move(next_tag_data));
    }
    return lst;
}

Compound BinaryDeserializer::deserialize_compound() {
    Compound cmpd;
    while (true) {
        auto [next_name, next_tag] = deserialize();
        if (next_tag.is<nbtview::End>()) {
            break;
        }
        cmpd.emplace(std::move(next_name), std::move(next_tag));
    }
    return cmpd;
}

template <typename T> std::vector<T> BinaryDeserializer::deserialize_array() {
    int32_t n_values = scanner.read<int32_t>();
    return scanner.read_array<T>(n_values);
}

std::string BinaryDeserializer::deserialize_string() {
    uint16_t bytes = scanner.read<uint16_t>();
    return scanner.read_string(bytes);
}

TagValue BinaryDeserializer::deserialize_typed_value(TypeCode type) {
    switch (type) {
    case TypeCode::Byte:
        return scanner.read<Byte>();
    case TypeCode::Short:
        return scanner.read<Short>();
    case TypeCode::Int:
        return scanner.read<Int>();
    case TypeCode::Long:
        return scanner.read<Long>();
    case TypeCode::Float:
        return scanner.read<Float>();
    case TypeCode::Double:
        return scanner.read<Double>();
    case TypeCode::Byte_Array:
        return deserialize_array<Byte>();
    case TypeCode::String:
        return deserialize_string();
    case TypeCode::List:
        return deserialize_list();
    case TypeCode::Compound:
        return deserialize_compound();
    case TypeCode::Int_Array:
        return deserialize_array<Int>();
    case TypeCode::Long_Array:
        return deserialize_array<Long>();
    default:
        throw std::runtime_error("Unhandled tag type");
    }
}

} // namespace nbtview
