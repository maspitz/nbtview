// nbtshow.cpp

#include <fstream>
#include <iostream>
#include <vector>

#include <zlib.h>

#include "nbtview.hpp"

namespace nbt = nbtview;

std::vector<unsigned char>
decompress_gzip(std::vector<unsigned char> &compressed_data) {
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

std::vector<unsigned char> read_file(const std::string &filename) {
    // Get the file size
    std::ifstream file(filename,
                       std::ios::binary | std::ios::in | std::ios::ate);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the entire file into a vector<unsigned char>
    std::vector<unsigned char> bytes(size);

    file.read(reinterpret_cast<char *>(bytes.data()), size);
    return bytes;
}

bool has_gzip_header(std::vector<unsigned char> &data) {
    if (data.size() < 4) {
        return false;
    }
    return (data[0] == 0x1f) && (data[1] == 0x8b) && (data[2] == 0x08);
}

int main(int argc, const char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " filename search-key"
                  << std::endl;
        return -1;
    }
    std::string filename(argv[1]);
    std::string key(argv[2]);

    auto bytes = read_file(filename);
    if (bytes.size() < 8) {
        std::cout << "Short file: exiting" << std::endl;
        return 0;
    }

    if (has_gzip_header(bytes)) {
        bytes = decompress_gzip(bytes);
    } 

    auto ret = nbt::fast_find_named_tag(bytes.begin(), bytes.end(),
                                        nbt::tagtype::TAG_Int, key);
    if (ret == bytes.end()) {
        std::cout << "Did not find: '" << key << "'" << std::endl;
    } else {
        std::cout << "Found '" << key << "' at " << ret - bytes.begin()
                  << std::endl;
    }

    return 0;
}
