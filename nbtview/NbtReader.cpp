// NbtReader.cpp

#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "BinaryDeserializer.hpp"
#include "NbtReader.hpp"
#include "Tag.hpp"
#include "zlib_utils.hpp"

namespace nbtview {

Tag NbtReader::read_from_file(const std::string &filename) {
    std::ifstream file(filename, std::ifstream::binary);

    if (!file) {
        std::cerr << "File " << filename << " could not be opened."
                  << std::endl;
        exit(0);
    }
    return read_from_stream(file);
}

Tag NbtReader::read_from_stream(std::istream &input) {
    // Get stream size
    input.seekg(0, input.end);
    std::streampos n_bytes = input.tellg();
    input.seekg(0, input.beg);

    // Read stream into vector of bytes
    std::vector<unsigned char> bytes(n_bytes);
    input.read(reinterpret_cast<char *>(bytes.data()), n_bytes);
    return read_from_bytes(std::move(bytes));
}

Tag NbtReader::read_from_bytes(std::vector<unsigned char> bytes) {
    if (has_gzip_header(bytes)) {
        bytes = decompress_gzip(bytes);
    }
    BinaryDeserializer reader(std::move(bytes));
    auto root_data = reader.deserialize();
    auto &root_name = root_data.first;
    return std::move(root_data.second);
}

} // namespace nbtview
