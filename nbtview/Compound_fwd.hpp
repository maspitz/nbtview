// Compound_fwd.hpp

#ifndef NBT_COMPOUND_FWD_H_
#define NBT_COMPOUND_FWD_H_

#include <map>
#include <string>

namespace nbtview {

class Tag;

class Compound {

  private:
    std::map<std::string, Tag> tags;

  public:
    auto size() { return tags.size(); }
    std::string to_string() const;

    template <typename T> bool contains(const std::string &name) const;

    template <typename T> T &at(const std::string &name);

    template <class... Types>
    std::pair<typename std::map<std::string, Tag>::iterator, bool>
    emplace(Types &&...args) {
        return tags.emplace(std::forward<Types>(args)...);
    }
    std::map<std::string, Tag>::const_iterator cbegin() const {
        return tags.cbegin();
    }
    std::map<std::string, Tag>::const_iterator cend() const {
        return tags.cend();
    }
};

} // namespace nbtview

#endif // NBT_COMPOUND_FWD_H_
