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
  private:
    std::vector<Tag> tags;
    TypeCode list_type_;

  public:
    using size_type = std::vector<Tag>::size_type;

    List(BinaryScanner &s);
    std::string to_string() const;
    TypeCode list_type() const { return list_type_; }
    template <typename T> T &at(size_type pos);
    size_type size();
};

} // namespace nbtview

#endif // NBT_LIST_FWD_H_
