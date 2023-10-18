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

#include <utility>
#include <vector>

namespace nbtview {

bool has_compression_header(const unsigned char *data, size_t data_length);

//! Decompress data into a vector of bytes.
std::vector<unsigned char> decompress_data(const unsigned char *compressed_data,
                                           size_t data_length);

struct Inflation_Status {
    bool complete;
    bool corrupt;
    int corrupt_sector;
};

//! Decompress compressed data into a vector of bytes.
std::pair<std::vector<unsigned char>, Inflation_Status>
inflate_sectors(const unsigned char *input_data, size_t input_length);

} // namespace nbtview

#endif // ZLIB_UTILS_H_
