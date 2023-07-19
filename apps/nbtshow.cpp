// nbtshow.cpp

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "NbtReader.hpp"
#include "Tag.hpp"
#include "nbtview.hpp"
#include "tag_utilities.hpp"

namespace nbt = nbtview;

int32_t extract_int32(const std::vector<unsigned char> &data,
                      std::vector<unsigned char>::const_iterator &iter) {
    // Check if data has sufficient length
    auto end_offset = data.end() - iter;
    if (end_offset < 4) {
        throw std::runtime_error("Insufficient data length");
    }
    return __builtin_bswap32(*reinterpret_cast<const int32_t *>(&(*iter)));
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
                                        nbt::TypeCode::Int, name);
    if (ret != data.end()) {
        auto val = extract_int32(data, ret);
        std::cout << "Found '" << name << " = " << val << " at "
                  << ret - data.begin() << std::endl;
    }
}

void print_long_tag(std::vector<unsigned char> &data, const std::string &name) {
    auto ret = nbt::fast_find_named_tag(data.begin(), data.end(),
                                        nbt::TypeCode::Long, name);
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
    std::ifstream infile(filename);

    auto [root_name, root_tag] = nbt::read_binary(infile);
    std::cout << "root_name: " << root_name << std::endl;
    std::cout << "root_tag: " << nbt::tag_to_string(root_tag) << std::endl;

    return 0;
}
