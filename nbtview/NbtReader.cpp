// NbtReader.cpp

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <zlib.h>

#include "BinaryScanner.hpp"
#include "NbtReader.hpp"

namespace nbtview {

bool has_gzip_header(const std::vector<unsigned char> &data) {
    if (data.size() < 4) {
        return false;
    }
    return (data[0] == 0x1f) && (data[1] == 0x8b) && (data[2] == 0x08);
}

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

Compound NbtReader::read_from_file(const std::string &filename) {
    std::ifstream file(filename, std::ifstream::binary);

    if (!file) {
        std::cerr << "File " << filename << " could not be opened."
                  << std::endl;
        exit(0);
    }
    return read_from_stream(file);
}

Compound NbtReader::read_from_stream(std::ifstream &input) {
    // Get stream size
    input.seekg(0, input.end);
    std::streampos n_bytes = input.tellg();
    input.seekg(0, input.beg);

    // Read stream into vector of bytes
    std::vector<unsigned char> bytes(n_bytes);
    input.read(reinterpret_cast<char *>(bytes.data()), n_bytes);
    return read_from_bytes(std::move(bytes));
}

Compound NbtReader::read_from_bytes(std::vector<unsigned char> bytes) {
    if (has_gzip_header(bytes)) {
        bytes = decompress_gzip(bytes);
    }
    BinaryScanner s(std::move(bytes));
    auto root_type = static_cast<TypeCode>(s.get_value<int8_t>());
    if (root_type != TypeCode::Compound) {
        throw std::runtime_error("Root tag is not a compound tag");
    }
    auto root_name = s.get_string();
    return Compound(s);
}

} // namespace nbtview
