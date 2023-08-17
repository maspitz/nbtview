/**
 * @file zlib_utils.hpp
 * @brief Functions to interface with zlib compression
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef ZLIB_UTILS_H_
#define ZLIB_UTILS_H_

#include <span>
#include <vector>

namespace nbtview {

bool has_compression_header(const std::span<unsigned char> data);

//! Decompress data into a vector of bytes.
std::vector<unsigned char>
decompress_data(std::vector<unsigned char> &compressed_data);

//! Decompress a vector of sectors into a vector of bytes.
int inflate_sectors(
    const std::vector<std::span<unsigned char>> &compressed_sectors,
    std::vector<unsigned char> &decompressed_data);

} // namespace nbtview

#endif // ZLIB_UTILS_H_
