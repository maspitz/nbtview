// Compound.hpp

#ifndef NBT_COMPOUND_H_
#define NBT_COMPOUND_H_

#include <map>
#include <memory>
#include <string>
#include <variant>

#include "List.hpp"

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

template <typename T> bool Compound::contains(const std::string &name) const {
    auto it = data->find(name);
    if (it == data->end()) {
        return false;
    }
    if (!std::holds_alternative<T>(it->second)) {
        return false;
    }
    return true;
}

template <typename T> void Compound::put(const std::string &name, T &&value) {
    data->insert_or_assign(name, std::move(value));
}

template <typename T> T &Compound::get(const std::string &name) {
    return std::get<T>(data->at(name));
}

} // namespace nbtview

#endif // NBT_COMPOUND_H_
