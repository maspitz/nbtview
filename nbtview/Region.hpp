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
#include <iosfwd>
#include <vector>

namespace nbtview {

/**
 * @brief Region represents the Region files that contain NBT chunk data
 *
 * This class has no ownership over chunk data, nor of the underlying data
 * store.
 *
 * Its concern is with the sector layout of chunk data, and to some extent,
 * metadata such as modification timestamps.
 * */
class Region {
  public:
    //! chunk width in blocks
    static const int chunk_width = 16;

    //! region width in chunks
    static const int region_width = 32;

    //! total number of chunks per region
    static const int chunk_count = region_width * region_width;

    //! length of a region data sector in bytes
    static const int sector_length = 4096;

    struct ChunkData {
        uint32_t timestamp;
        uint32_t offset;
        uint8_t length;
    };

  private:
    std::array<ChunkData, chunk_count> chunk;
    std::vector<bool> sector_free;

  public:
    /**
     * @brief Initializes a Region with zeroed-out metadata.
     */
    Region() {}
    /**
     * @brief Loads the timestamp data by reading 4 KiB from the given input
     * stream.
     * */
    std::istream &ReadTimestamps(std::istream &input);
    /**
     * @brief Loads the chunk data sector offset and length data by reading 4
     * KiB from the given input stream.
     * */
    std::istream &ReadOffsets(std::istream &input);
    // TODO: consider using bounds-checking access
    //! Returns the timestamp of the given chunk
    uint32_t get_timestamp(int idx) { return chunk[idx].timestamp; }
    //! Returns the sector offset for the given chunk
    uint32_t get_offset(int idx) { return chunk[idx].offset; }
    //! Returns the sector length for the given chunk
    uint32_t get_length(int idx) { return chunk[idx].length; }

    //! Returns undecoded chunk data as a vector of bytes.  input must be set to
    //! beginning of region file.
    std::vector<unsigned char> get_chunk_data(std::istream &input, int idx);
};

} // namespace nbtview

#endif // NBT_REGION_H_
