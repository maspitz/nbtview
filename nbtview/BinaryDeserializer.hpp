// Binarydeserializer.hpp

#include <memory>

#include "BinaryScanner.hpp"
#include "Deserializer.hpp"
#include "Tag.hpp"

#ifndef BINARYDESERIALIZER_H_
#define BINARYDESERIALIZER_H_

#include <memory>
#include <utility>
#include <vector>

#include "Deserializer.hpp"

namespace nbtview {

class BinaryScanner;

class BinaryDeserializer : public Deserializer {
  private:
    std::unique_ptr<BinaryScanner> scanner_;

  public:
    BinaryDeserializer(std::vector<unsigned char> bytes);

    std::pair<std::string, Tag> deserialize() override;

  private:
    List deserialize_list();

    Compound deserialize_compound();

    Tag deserialize_typed_value(TypeCode type);
};

} // namespace nbtview
#endif // BINARYDESERIALIZER_H_
