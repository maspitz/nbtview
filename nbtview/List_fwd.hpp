// List_fwd.hpp
#ifndef NBT_LIST_FWD_H_
#define NBT_LIST_FWD_H_

#include <memory>
#include <vector>

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

    void push_back(int j) { data->push_back(j); }

    TypeCode list_type() { return list_type_; }

  private:
    std::unique_ptr<std::vector<Tag>> data;
    TypeCode list_type_;
};

} // namespace nbtview

#endif // NBT_LIST_FWD_H_
