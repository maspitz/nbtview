// Region.cpp

#include <fstream>
#include <ios>
#include <span>
#include <string>
#include <vector>

#include "Region.hpp"

namespace nbtview {

void Region::load_from_sectors(const Sector_Data &offsets,
                               const Sector_Data &timestamps) {
    if (offsets.size() != sector_length || timestamps.size() != sector_length) {
        throw std::runtime_error("Improper sector length");
    }
    for (int i = 0; i < sector_length; i += 4) {
        chunk[i / 4].offset =
            (offsets[i] << 16) + (offsets[i + 1] << 8) + (offsets[i + 2]);
        chunk[i / 4].length = offsets[i + 3];
        chunk[i / 4].timestamp = (timestamps[i] << 24) +
                                 (timestamps[i + 1] << 16) +
                                 (timestamps[i + 2] << 8) + (timestamps[i + 3]);
    }
}

void Region::save_to_sectors(Sector_Data &offsets,
                             Sector_Data &timestamps) const {
    if (offsets.size() != sector_length || timestamps.size() != sector_length) {
        throw std::runtime_error("Improper sector length");
    }
    for (int i = 0; i < sector_length; i += 4) {
        offsets[i] = (chunk[i / 4].offset >> 16) & 0xFF;
        offsets[i + 1] = (chunk[i / 4].offset >> 8) & 0xFF;
        offsets[i + 2] = chunk[i / 4].offset & 0xFF;
        offsets[i + 3] = chunk[i / 4].length;

        timestamps[i] = (chunk[i / 4].timestamp >> 24) & 0xFF;
        timestamps[i + 1] = (chunk[i / 4].timestamp >> 16) & 0xFF;
        timestamps[i + 2] = (chunk[i / 4].timestamp >> 8) & 0xFF;
        timestamps[i + 3] = chunk[i / 4].timestamp & 0xFF;
    }
}

Region_File::Region_File(const std::string &filename)
    : name(filename), file(filename) {
    metadata.load_from_sectors(read_sector(0), read_sector(1));
}

Region::Sector_Data Region_File::read_sector(int sector_index) {
    Region::Sector_Data sector(Region::sector_length);
    file.seekg(Region::sector_length * sector_index, std::ios::beg);
    file.read(reinterpret_cast<char *>(sector.data()), sector.size());
    if (!file) {
        throw std::runtime_error("Could not read sector " +
                                 std::to_string(sector_index) +
                                 " of region file " + name);
    }
    return sector;
}

uint32_t chunk_data_length(std::span<unsigned char> chunk_header) {
    if (chunk_header.size() < 5) {
        throw std::runtime_error("Chunk header is too short");
    }
    uint32_t length = (chunk_header[0] << 24) + (chunk_header[1] << 16) +
                      (chunk_header[2] << 8) + chunk_header[3];
    uint8_t compression_type = chunk_header[4];
    if (compression_type > 3) {
        throw std::runtime_error("Chunk header has unknown compression type.");
    }
    return length;
}

std::vector<unsigned char> Region_File::get_chunk_data(int chunk_index) {
    uint32_t sector_offset = chunk_offset(chunk_index);
    uint8_t sector_count = chunk_length(chunk_index);
    if (sector_count == 0) {
        return std::vector<unsigned char>{};
    }

    Region::Sector_Data first_sector = read_sector(sector_offset);

    uint32_t data_length = chunk_data_length(first_sector);

    if ((data_length + 5) > sector_count * Region::sector_length) {
        throw std::runtime_error("Reported encoded chunk length exceeds "
                                 "allocated sectors for chunk");
    }

    std::vector<unsigned char> chunk_data(data_length);
    file.seekg(Region::sector_length * sector_offset + 5, std::ios::beg);
    file.read(reinterpret_cast<char *>(chunk_data.data()), data_length);
    if (!file) {
        throw std::runtime_error("Input error when reading chunk data");
    }

    return chunk_data;
}

} // namespace nbtview
