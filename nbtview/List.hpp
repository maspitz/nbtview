// List.hpp
#ifndef NBT_LIST_H_
#define NBT_LIST_H_

#include <string>
#include <vector>

#include "Tag.hpp"

class BinaryScanner;

namespace nbtview {

struct List {
  public:
    std::vector<Tag> tags;
    TypeCode list_type;

    List(BinaryScanner &s);
    std::string to_string() const;
};

} // namespace nbtview

#endif // NBT_LIST_H_
