// List_fwd.hpp
#ifndef NBT_LIST_FWD_H_
#define NBT_LIST_FWD_H_

#include <string>
#include <vector>

#include "nbtview.hpp"

namespace nbtview {

class BinaryScanner;
class Tag;

class List {
  public:
    std::vector<Tag> tags;
    TypeCode list_type;

    List(BinaryScanner &s);
    std::string to_string() const;
};

} // namespace nbtview

#endif // NBT_LIST_FWD_H_
