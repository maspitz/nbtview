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

int32_t extract_int32(const std::vector<unsigned char> &data,
                      std::vector<unsigned char>::const_iterator &iter) {
    // Check if data has sufficient length
    auto end_offset = data.end() - iter;
    if (end_offset < 4) {
        throw std::runtime_error("Insufficient data length");
    }
    return __builtin_bswap32(*reinterpret_cast<const int32_t *>(&(*iter)));

    /*    int32_t result = (data[offset] << 24) | (data[offset + 1] << 16) |
                         (data[offset + 2] << 8) | data[offset + 3];
    */
}

int64_t extract_int64(const std::vector<unsigned char> &data,
                      std::vector<unsigned char>::const_iterator &iter) {
    // Check if data has sufficient length
    auto end_offset = data.end() - iter;
    if (end_offset < 8) {
        throw std::runtime_error("Insufficient data length");
    }

    return __builtin_bswap64(*reinterpret_cast<const int64_t *>(&(*iter)));
}

void print_int_tag(std::vector<unsigned char> &data, const std::string &name) {
    auto ret = nbt::fast_find_named_tag(data.begin(), data.end(),
                                        nbt::Tag::Type::Int, name);
    if (ret != data.end()) {
        auto val = extract_int32(data, ret);
        std::cout << "Found '" << name << " = " << val << " at "
                  << ret - data.begin() << std::endl;
    }
}

void print_long_tag(std::vector<unsigned char> &data, const std::string &name) {
    auto ret = nbt::fast_find_named_tag(data.begin(), data.end(),
                                        nbt::Tag::Type::Long, name);
    if (ret != data.end()) {
        auto val = extract_int64(data, ret);
        std::cout << "Found '" << name << " = " << val << " at "
                  << ret - data.begin() << std::endl;
    }
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return -1;
    }
    std::string filename(argv[1]);

    auto bytes = read_file(filename);
    if (bytes.size() < 8) {
        std::cout << "Short file: exiting" << std::endl;
        return 0;
    }

    if (has_gzip_header(bytes)) {
        bytes = decompress_gzip(bytes);
    }

    std::vector<std::string> int_tags{"SpawnX", "SpawnY", "SpawnZ",
                                      "playerGameType", "DataVersion"};
    for (auto tag_name : int_tags) {
        print_int_tag(bytes, tag_name);
    }

    std::vector<std::string> long_tags{"RandomSeed", "LastPlayed", "Time"};
    for (auto tag_name : long_tags) {
        print_long_tag(bytes, tag_name);
    }

    auto bscr = nbtview::BinaryScanner(bytes);

    // auto root_tag = nbtview::make_tag(bscr);
    // std::cout << "root_tag: " << root_tag->to_string() << std::endl;

    return 0;
}
