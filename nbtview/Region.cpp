// Region.cpp

#include <istream>

#include "BinaryReader.hpp"
#include "Region.hpp"

namespace nbtview {

std::istream &Region::ReadTimestamps(std::istream &input) {
    std::vector<unsigned char> bytes(SECTOR_LENGTH);
    input.read(reinterpret_cast<char *>(bytes.data()), SECTOR_LENGTH);
    BinaryReader reader(std::move(bytes));
    for (int i = 0; i < N_CHUNKS; ++i) {
        chunk[i].timestamp = reader.get_value<uint32_t>();
    }
    return input;
}

std::istream &Region::ReadOffsets(std::istream &input) {
    std::vector<unsigned char> bytes(SECTOR_LENGTH);
    input.read(reinterpret_cast<char *>(bytes.data()), SECTOR_LENGTH);
    BinaryReader reader(std::move(bytes));
    for (int i = 0; i < N_CHUNKS; ++i) {
        uint32_t data = reader.get_value<uint32_t>();
        chunk[i].length = data & 0xff;
        chunk[i].offset = data >> 8;
    }
    return input;
}

} // namespace nbtview
