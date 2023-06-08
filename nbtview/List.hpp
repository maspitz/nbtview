// List.hpp
#ifndef NBT_LIST_H_
#define NBT_LIST_H_

#include <string>
#include <vector>

#include "Tag.hpp"

namespace nbtview {

struct List : public std::vector<Tag> {
  public:
    TypeCode list_type;

    List(BinaryScanner &s);
    std::string to_string();
};

} // namespace nbtview

#endif // NBT_LIST_H_
