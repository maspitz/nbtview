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
    //! chunk width in blocks
    static const int chunk_width = 16;

    //! region width in chunks
    static const int region_width = 32;

    //! total number of chunks per region
    static const int chunk_count = region_width * region_width;

    //! length of a region file data sector in bytes
    static const int sector_length = 4096;

    using Sector_Data = std::array<unsigned char, sector_length>;

    struct Chunk_Data {
        uint32_t offset;
        uint8_t length;
        uint32_t timestamp;
    };

    std::array<Chunk_Data, chunk_count> chunk;

    void load_from_sectors(const Sector_Data &offsets,
                           const Sector_Data &timestamps);
    void save_to_sectors(Sector_Data &offsets, Sector_Data &timestamps) const;
};

/**
 * @brief Region_File provides access to a Region file that contains NBT chunk
 * data
 * */
class Region_File {
  public:
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

    Region::Sector_Data read_sector(int sector_index);
};

} // namespace nbtview

#endif // NBT_REGION_H_
