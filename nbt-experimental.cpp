#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

int main() { return 0; }

namespace nbt {

struct basic_nbt {
  // NBT data types
  using compound_t = std::map<std::string, basic_nbt>;
  using list_t = std::vector<basic_nbt>;
  using short_t = int16_t;
  using string_t = std::string;
  using float_t = float;

  // NBT type ids
  enum class nbt_type : uint8_t {
    null,
    number_short,
    number_float,
    string,
    compound,
    list
  };

  // NBT value storage
  // values requiring more than 8 bytes are stored using bare pointers
  union nbt_value {
    compound_t *compound_ptr;
    list_t *list_ptr;
    string_t *string_ptr;
    short_t short_val;
    float_t float_val;

    // default ctor: null value
    nbt_value() = default;
    // ctors for primitive data types
    nbt_value(short_t val) noexcept : short_val(val) {}
    nbt_value(float_t val) noexcept : float_val(val) {}

    // ctors for class data types: deep copy
    nbt_value(const std::string &val) : string_ptr(new std::string(val)) {}
    nbt_value(const compound_t &val) : compound_ptr(new compound_t(val)) {}
    nbt_value(const list_t &val) : list_ptr(new list_t(val)) {}

    // ctor for default value using type id
    // nbt_value(nbt_type id) { // switch/case }
  };

  nbt_type m_type;
  nbt_value m_value;

  basic_nbt() : m_type(nbt_type::null), m_value{} {}
  ~basic_nbt() {
    switch (m_type) {
    case nbt_type::string:
      delete m_value.string_ptr;
      m_value.string_ptr = nullptr;
      break;
    case nbt_type::compound:
      delete m_value.compound_ptr;
      m_value.compound_ptr = nullptr;
      break;
    case nbt_type::list:
      delete m_value.list_ptr;
      m_value.list_ptr = nullptr;
      break;
    default:
      break;
    }
  }
};
} // namespace nbt
