// Region.cpp

#include <fstream>
#include <ios>
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
    metadata.load_from_sectors(read_sectors(0, 1), read_sectors(1, 1));
}

Region::Sector_Data Region_File::read_sectors(int sector_index,
                                              int sector_count) {
    return read_data(Region::sector_length * sector_index,
                     Region::sector_length * sector_count);
}

Region::Sector_Data Region_File::read_data(uint64_t offset,
                                           size_t data_length) {
    Region::Sector_Data buffer(data_length);
    file.seekg(offset, std::ios::beg);
    file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
    if (!file) {
        throw std::runtime_error("Could not read from offset " +
                                 std::to_string(offset) + " of region file " +
                                 name);
    }
    return buffer;
}

uint32_t chunk_data_length(const std::vector<unsigned char> &chunk_header) {
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

    const int chunk_header_length = 5;
    uint64_t chunk_offset = Region::sector_length * sector_offset;
    auto chunk_header = read_data(chunk_offset, chunk_header_length);
    uint32_t data_length = chunk_data_length(chunk_header);

    if ((data_length + 5) > sector_count * Region::sector_length) {
        throw std::runtime_error("Reported encoded chunk length exceeds "
                                 "allocated sectors for chunk");
    }

    auto chunk_data =
        read_data(chunk_offset + chunk_header_length, data_length);
    return chunk_data;
}

} // namespace nbtview
