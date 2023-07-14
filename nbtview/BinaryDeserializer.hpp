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

#include <memory>
#include <utility>
#include <vector>

#include "Deserializer.hpp"
#include "Tag.hpp"

namespace nbtview {

class BinaryReader;

class BinaryDeserializer : public Deserializer {
  private:
    std::unique_ptr<BinaryReader> scanner;

  public:
    BinaryDeserializer(std::vector<unsigned char> bytes);
    ~BinaryDeserializer();

    std::pair<std::string, Tag> deserialize() override;

  private:
    List deserialize_list();

    Compound deserialize_compound();

    Tag deserialize_typed_value(TypeCode type);
};

} // namespace nbtview
#endif // BINARYDESERIALIZER_H_
