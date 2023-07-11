// Compound_fwd.hpp

#ifndef NBT_COMPOUND_FWD_H_
#define NBT_COMPOUND_FWD_H_

#include <map>
#include <memory>
#include <string>

#include "Tag.hpp"

namespace nbtview {

class Compound {
  public:
    Compound() : data(std::make_unique<std::map<std::string, Tag>>()) {}

    using iterator = std::map<std::string, Tag>::iterator;
    using const_iterator = std::map<std::string, Tag>::const_iterator;

    auto begin() { return data->begin(); }
    auto end() { return data->end(); }
    auto begin() const { return data->begin(); }
    auto end() const { return data->end(); }
    auto cbegin() const { return data->cbegin(); }
    auto cend() const { return data->cend(); }

    size_t size() const { return data->size(); }
    bool empty() const { return data->empty(); }

    Tag &at(const std::string &key) { return data->at(key); }
    const Tag &at(const std::string &key) const { return data->at(key); }

    // construct element directly in the Compound
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args &&...args) {
        return data->emplace(std::forward<Args>(args)...);
    }

    template <typename T> bool contains(const std::string &name) const;

    template <typename T> void put(const std::string &name, T &&value);

    template <typename T> T &get(const std::string &name);

  private:
    std::unique_ptr<std::map<std::string, Tag>> data;
};

} // namespace nbtview

#endif // NBT_COMPOUND_FWD_H_
