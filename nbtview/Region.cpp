// Region.cpp

#include <ios>
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

std::vector<unsigned char> Region::get_chunk_bytes(int idx,
                                                   std::istream &input) {
    int byte_offset = get_offset(idx) * SECTOR_LENGTH;
    int byte_length = get_length(idx) * SECTOR_LENGTH;
    uint8_t sector_header[5];
    input.seekg(byte_offset, std::ios_base::cur);
    input.read(reinterpret_cast<char *>(sector_header), 5);
    uint32_t chunk_byte_length = sector_header[3] + (sector_header[2] << 8) +
                                 (sector_header[1] << 16) +
                                 (sector_header[0] << 24);
    uint8_t compression_type = sector_header[4];
    std::vector<unsigned char> chunk_bytes(chunk_byte_length);
    input.read(reinterpret_cast<char *>(chunk_bytes.data()), chunk_byte_length);
    if (!input) {
        throw(std::runtime_error("Failed to read chunk data"));
    }
    return chunk_bytes;
}

} // namespace nbtview
