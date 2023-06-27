// NbtReader.hpp

#ifndef NBTREADER_H_
#define NBTREADER_H_

#include <fstream>
#include <string>
#include <vector>

#include "Compound.hpp"

namespace nbtview {
class NbtReader {
  public:
    static Tag read_from_file(const std::string &filename);
    static Tag read_from_bytes(std::vector<unsigned char> bytes);
    static Tag read_from_stream(std::ifstream &input);
};

} // namespace nbtview

#endif // NBTREADER_H_
