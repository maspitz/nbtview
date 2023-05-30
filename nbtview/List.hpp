// List.hpp
#ifndef NBT_LIST_H_
#define NBT_LIST_H_

#include <string>
#include <vector>

namespace nbtview {

struct Tag;

struct List : public std::vector<Tag> {
  public:
    using base = std::vector<Tag>;

    // inherit constructors from std::vector
    using base::base;

    std::string to_string();
};

} // namespace nbtview

#endif // NBT_LIST_H_
