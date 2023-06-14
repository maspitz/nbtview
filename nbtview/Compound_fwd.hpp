// Compound_fwd.hpp

#ifndef NBT_COMPOUND_FWD_H_
#define NBT_COMPOUND_FWD_H_

#include <map>
#include <string>

namespace nbtview {

class BinaryScanner;
class Tag;

class Compound {

  private:
    std::map<std::string, Tag> tags;

  public:
    Compound(BinaryScanner &s);

    auto size() { return tags.size(); }
    std::string to_string() const;

    template <typename T> bool contains(const std::string &name) const;

    template <typename T> T &at(const std::string &name);
};

} // namespace nbtview

#endif // NBT_COMPOUND_FWD_H_
