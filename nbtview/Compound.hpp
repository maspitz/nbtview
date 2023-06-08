// Compound.hpp

#ifndef NBT_COMPOUND_H_
#define NBT_COMPOUND_H_

#include "Tag.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace nbtview {

struct Compound {

  public:
    std::map<std::string, Tag> tags;

    Compound(BinaryScanner &s);

    std::optional<Byte> get_Byte(const std::string &name) const {
        return get_value<Byte>(name);
    }

    std::optional<Short> get_Short(const std::string &name) const {
        return get_value<Short>(name);
    }

    std::optional<Int> get_Int(const std::string &name) const {
        return get_value<Int>(name);
    }

    std::optional<Long> get_Long(const std::string &name) const {
        return get_value<Long>(name);
    }

    std::optional<Float> get_Float(const std::string &name) const {
        return get_value<Float>(name);
    }

    std::optional<Double> get_Double(const std::string &name) const {
        return get_value<Double>(name);
    }

    const Byte_Array *get_Byte_Array(const std::string &name) const {
        return get_ptr<Byte_Array>(name);
    }

    std::optional<String> get_String(const std::string &name) const {
        return get_value<String>(name);
    }

    const List *get_List(const std::string &name) const {
        return get_ptr<List>(name);
    }

    const Compound *get_Compound(const std::string &name) const {
        return get_ptr<Compound>(name);
    }

    const Int_Array *get_Int_Array(const std::string &name) const {
        return get_ptr<Int_Array>(name);
    }

    const Long_Array *get_Long_Array(const std::string &name) const {
        return get_ptr<Long_Array>(name);
    }

    std::string to_string() const;

  private:
    template <typename T>
    std::optional<T> get_value(const std::string &name) const {
        auto it = tags.find(name);
        if (it == tags.end()) {
            return std::nullopt;
        }
        if (!std::holds_alternative<T>(it->second.data)) {
            return std::nullopt;
        }
        return std::get<T>(it->second.data);
    }

    template <typename T> const T *get_ptr(const std::string &name) const {
        auto it = tags.find(name);
        if (it == tags.end()) {
            return nullptr;
        }
        if (!std::holds_alternative<std::unique_ptr<T>>(it->second.data)) {
            return nullptr;
        }
        return std::get<std::unique_ptr<T>>(it->second.data).get();
    }
};

} // namespace nbtview

#endif // NBT_COMPOUND_H_
