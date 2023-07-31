// Region.cpp

#include <istream>

#include "BinaryReader.hpp"
#include "Region.hpp"

namespace nbtview {

std::istream &Region::ReadTimestamps(std::istream &input) {
    std::vector<unsigned char> bytes(SECTOR_LENGTH);
    auto init_pos = input.tellg();
    input.seekg(SECTOR_LENGTH, std::ios::cur);
    input.read(reinterpret_cast<char *>(bytes.data()), SECTOR_LENGTH);
    BinaryReader reader(std::move(bytes));
    for (int i = 0; i < N_CHUNKS; ++i) {
        chunk[i].timestamp = reader.get_value<uint32_t>();
    }
    input.seekg(init_pos, std::ios::beg);
    return input;
}

std::istream &Region::ReadOffsets(std::istream &input) {
    std::vector<unsigned char> bytes(SECTOR_LENGTH);
    auto init_pos = input.tellg();
    input.read(reinterpret_cast<char *>(bytes.data()), SECTOR_LENGTH);
    BinaryReader reader(std::move(bytes));
    for (int i = 0; i < N_CHUNKS; ++i) {
        uint32_t data = reader.get_value<uint32_t>();
        chunk[i].length = data & 0xff;
        chunk[i].offset = data >> 8;
    }
    input.seekg(init_pos, std::ios::beg);
    return input;
}

std::vector<unsigned char> Region::get_chunk_data(std::istream &input,
                                                  int idx) {
    auto init_pos = input.tellg();
    input.seekg(SECTOR_LENGTH * get_offset(idx), std::ios::cur);

    const int HEADER_LENGTH = 5;
    std::vector<unsigned char> chunk_header(HEADER_LENGTH);
    input.read(reinterpret_cast<char *>(chunk_header.data()), HEADER_LENGTH);

    uint32_t chunk_data_length = (chunk_header[0] << 24) +
                                 (chunk_header[1] << 16) +
                                 (chunk_header[2] << 8) + chunk_header[3];
    uint8_t compression_type = chunk_header[4];
    if (compression_type > 3) {
        throw std::runtime_error(
            "Unknown compression type in reading chunk data");
    }

    if (chunk_data_length < 0 ||
        chunk_data_length / SECTOR_LENGTH > get_length(idx)) {
        throw std::runtime_error(
            "Reported chunk length exceeds allocated space for chunk");
    }

    std::vector<unsigned char> chunk_data(chunk_data_length);
    input.read(reinterpret_cast<char *>(chunk_data.data()), chunk_data_length);
    input.seekg(init_pos, std::ios::beg);
    if (!input) {
        throw std::runtime_error("Input error when reading chunk data");
    }
    return chunk_data;
}

} // namespace nbtview
