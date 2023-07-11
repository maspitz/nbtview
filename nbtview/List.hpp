// List.hpp
#ifndef NBT_LIST_H_
#define NBT_LIST_H_

#include <memory>
#include <variant>
#include <vector>

#include "Compound.hpp"

#include "Tag.hpp"

namespace nbtview {

class List {
  public:
    List(TypeCode type)
        : data(std::make_unique<std::vector<Tag>>()), list_type_(type) {}

    auto begin() { return data->begin(); }
    auto end() { return data->end(); }
    auto begin() const { return data->begin(); }
    auto end() const { return data->end(); }
    auto cbegin() const { return data->cbegin(); }
    auto cend() const { return data->cend(); }

    size_t size() const { return data->size(); }
    bool empty() const { return data->empty(); }
    void reserve(size_t new_cap) { data->reserve(new_cap); }

    Tag &at(size_t position) { return data->at(position); }
    const Tag &at(size_t position) const { return data->at(position); }

    // construct element directly in the List
    template <typename... Args> void emplace_back(Args &&...args) {
        data->emplace_back(std::forward<Args>(args)...);
    }

    template <typename T> T &get(size_t pos);

    void push_back(Tag &&t);

    TypeCode list_type() const { return list_type_; }

  private:
    std::unique_ptr<std::vector<Tag>> data;
    TypeCode list_type_;
};

template <typename T> T &List::get(size_t pos) {
    return std::get<T>(data->at(pos));
}

} // namespace nbtview

#endif // NBT_LIST_H_
