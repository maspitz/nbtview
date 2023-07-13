/**
 * @file Deserializer.hpp
 * @brief Abstract Interface for the deserialization of NBT data
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#include <string>
#include <utility>

#include "Tag.hpp"

#ifndef DESERIALIZER_H_
#define DESERIALIZER_H_

namespace nbtview {

// Builder design pattern: the concrete subclasses of Deserializer are
// responsible for building TagData objects.

class Deserializer {
  public:
    virtual ~Deserializer() = default;
    virtual std::pair<std::string, Tag> deserialize() = 0;
};

} // namespace nbtview

#endif // DESERIALIZER_H_
