// List_fwd.hpp
#ifndef NBT_LIST_FWD_H_
#define NBT_LIST_FWD_H_

#include <string>
#include <utility>
#include <vector>

#include "nbtview.hpp"

namespace nbtview {

class Tag;

class List {
  private:
    std::vector<Tag> tags;
    TypeCode list_type_;

  public:
    using size_type = std::vector<Tag>::size_type;

    List(TypeCode type, int32_t reserve_length) : list_type_(type) {
        tags.reserve(reserve_length);
    }

    template <class... Types> void emplace_back(Types &&...args) {
        tags.emplace_back(std::forward<Types>(args)...);
    }

    std::string to_string() const;
    TypeCode list_type() const { return list_type_; }
    template <typename T> T &at(size_type pos);
    size_type size();
};

} // namespace nbtview

#endif // NBT_LIST_FWD_H_
