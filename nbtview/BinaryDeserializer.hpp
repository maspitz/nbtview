/**
 * @file BinaryDeserializer.hpp
 * @brief Read binary encoded NBT data
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef BINARYDESERIALIZER_H_
#define BINARYDESERIALIZER_H_

#include <utility>

#include "BinaryReader.hpp"
#include "Deserializer.hpp"
#include "Tag.hpp"

namespace nbtview {

class BinaryDeserializer : public Deserializer {
  private:
    BinaryReader scanner;

  public:
    BinaryDeserializer(const unsigned char *buffer, size_t buffer_length)
        : scanner(buffer, buffer_length) {}
    ~BinaryDeserializer() = default;

    std::pair<std::string, TagData> deserialize() override;

  private:
    List deserialize_list();

    Compound deserialize_compound();

    template <typename T> std::vector<T> deserialize_array();
    std::string deserialize_string();

    TagData deserialize_typed_value(TypeCode type);
};

} // namespace nbtview
#endif // BINARYDESERIALIZER_H_
