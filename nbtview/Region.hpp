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
 * The Region class represents a container for chunk data, which is stored in
 * compressed NBT format.
 *
 *
 *
 * */
class Region {
  public:
    static const int CHUNK_WIDTH = 16;  // chunk width in blocks
    static const int REGION_WIDTH = 32; // region width in chunks
    static const int N_CHUNKS =
        REGION_WIDTH * REGION_WIDTH; // total number of chunks per region

    static const int SECTOR_LENGTH =
        4096; // length of a region data sector in bytes

    struct ChunkData {
        uint32_t timestamp;
        uint32_t offset;
        uint8_t length;
    };

  private:
    std::array<ChunkData, N_CHUNKS> chunk;
    std::vector<bool> sector_free;

  public:
    Region() {}
    std::istream &ReadTimestamps(std::istream &input);
    std::istream &ReadOffsets(std::istream &input);
};

} // namespace nbtview

#endif // NBT_REGION_H_
