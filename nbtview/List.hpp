// List.hpp
#ifndef NBT_LIST_H_
#define NBT_LIST_H_

#include "BinaryScanner.hpp"
#include "nbt_types.hpp"
#include <string>
#include <vector>

namespace nbtview {

struct List : public std::vector<Tag> {
  public:
    TypeCode list_type;

    List(BinaryScanner &s) : std::vector<Tag>() {
        list_type = static_cast<TypeCode>(s.get_value<int8_t>());
        auto list_length = s.get_value<int32_t>();
        this->reserve(list_length);
        for (int32_t i = 0; i < list_length; ++i) {
            this->emplace_back(Tag(s, list_type));
        }
    }
    std::string to_string();
};

} // namespace nbtview

#endif // NBT_LIST_H_
