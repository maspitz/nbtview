/**
 * @file NbtReader.hpp
 * @brief Interface to read NBT data from a binary source
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef NBTREADER_H_
#define NBTREADER_H_

#include <istream>
#include <string>
#include <vector>

#include "Tag.hpp"

namespace nbtview {
class NbtReader {
  public:
    static Tag read_from_file(const std::string &filename);
    static Tag read_from_bytes(std::vector<unsigned char> bytes);
    static Tag read_from_stream(std::istream &input);
};

} // namespace nbtview

#endif // NBTREADER_H_
