// zlib_utils.cpp

#include <iostream>
#include <vector>

#include <zlib.h>

#include "zlib_utils.hpp"

namespace nbtview {

bool has_compression_header(const std::vector<unsigned char> &data) {
    if (data.size() < 4) {
        return false;
    }

    // all possible ZLIB headers, given DEFLATE method and a 32KB window
    // [RFC1950]
    if (data[0] == 0x78 && (data[1] == 0x01 || data[1] == 0x5e ||
                            data[1] == 0x9c || data[1] == 0xda)) {
        return true;
    }

    // GZIP header bytes, given DEFLATE method [RFC1952]
    if (data[0] == 0x1f && data[1] == 0x8b && data[2] == 0x08) {
        return true;
    }

    return false;
}

std::vector<unsigned char>
decompress_data(std::vector<unsigned char> &compressed_data) {
    constexpr int buffer_size = 128;
    char buffer[buffer_size];

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = static_cast<uInt>(compressed_data.size());
    stream.next_in = reinterpret_cast<Bytef *>(compressed_data.data());

    constexpr int AUTO_HEADER_DETECTION = 32;

    if (inflateInit2(&stream, AUTO_HEADER_DETECTION | MAX_WBITS) != Z_OK) {
        std::cerr << "Failed to initialize zlib inflate" << std::endl;
        return {};
    }

    std::vector<unsigned char> decompressed_data;
    int ret;

    do {
        stream.avail_out = buffer_size;
        stream.next_out = reinterpret_cast<Bytef *>(buffer);

        ret = inflate(&stream, Z_NO_FLUSH);

        if (ret < 0) {
            std::cerr << "Error while decompressing: " << stream.msg
                      << std::endl;
            inflateEnd(&stream);
            return {};
        }

        int bytesRead = buffer_size - stream.avail_out;
        decompressed_data.insert(decompressed_data.end(), buffer,
                                 buffer + bytesRead);
    } while (ret != Z_STREAM_END);

    inflateEnd(&stream);

    return decompressed_data;
}

} // namespace nbtview
