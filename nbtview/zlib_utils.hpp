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

#include <vector>

namespace nbtview {

bool has_gzip_header(const std::vector<unsigned char> &data);

std::vector<unsigned char>
decompress_gzip(std::vector<unsigned char> &compressed_data);

} // namespace nbtview

#endif // ZLIB_UTILS_H_
