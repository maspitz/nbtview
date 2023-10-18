/**
 * @file Region.hpp
 * @brief Supports the Region format as a container of NBT data
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef NBT_REGION_H_
#define NBT_REGION_H_

#include <array>
#include <cstdint>
#include <fstream>
#include <vector>

namespace nbtview {

/**
 * @brief Region contains the metadata concerning the layout of encoded chunk
 * data.
 * */
struct Region {
    //! Chunk width in blocks
    static const int chunk_width = 16;

    //! Region width in chunks
    static const int region_width = 32;

    //! Total number of chunks per region
    static const int chunk_count = region_width * region_width;

    //! Length of a region file data sector in bytes
    static const int sector_length = 4096;

    //! Contains the bytes for a single sector
    using Sector_Data = std::vector<unsigned char>;

    struct Chunk_Data {
        uint32_t offset; // Index of the first sector holding this chunk's data
        uint8_t length;  // Number of sectors holding this chunk's data
        uint32_t timestamp; // Time when the chunk was last written out
    };

    //! Metadata for this region's chunks
    std::array<Chunk_Data, chunk_count> chunk;

    //! Loads chunk metadata from the two sectors of a region file header
    void load_from_sectors(const Sector_Data &offsets,
                           const Sector_Data &timestamps);

    //! Saves chunk metadata to two sectors suitable for a region file header
    void save_to_sectors(Sector_Data &offsets, Sector_Data &timestamps) const;
};

/**
 * @brief Region_File provides access to a Region file that contains NBT chunk
 * data
 * */
class Region_File {
  public:
    //! Opens a file and reads in the header data
    Region_File(const std::string &filename);

    //! Returns the sector offset for the given chunk
    uint32_t chunk_offset(int chunk_index) const {
        return metadata.chunk.at(chunk_index).offset;
    }

    //! Returns the sector length for the given chunk
    uint8_t chunk_length(int chunk_index) const {
        return metadata.chunk.at(chunk_index).length;
    }

    //! Returns the modification timestamp for the given chunk
    uint32_t chunk_timestamp(int chunk_index) const {
        return metadata.chunk.at(chunk_index).timestamp;
    }

    //! Returns the encoded data for the given chunk
    std::vector<unsigned char> get_chunk_data(int chunk_index);

  private:
    std::string name;
    std::ifstream file;
    Region metadata;

    Region::Sector_Data read_sectors(int sector_index, int sector_count);
    Region::Sector_Data read_data(uint64_t offset, size_t data_length);
};

} // namespace nbtview

#endif // NBT_REGION_H_
