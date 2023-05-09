// nbtshow.cpp

#include <fstream>
#include <iostream>
#include <vector>

#include "nbtview.hpp"

namespace nbt = nbtview;

int main(int argc, const char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " filename search-key"
                  << std::endl;
        return -1;
    }
    std::string filename(argv[1]);
    std::string key(argv[2]);

    // Get the file size
    std::ifstream file(filename,
                       std::ios::binary | std::ios::in | std::ios::ate);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the entire file into a vector of bytes
    std::vector<char> bytes(size);
    //    file.read(reinterpret_cast<char *>(bytes.data()), size);
    file.read(bytes.data(), size);

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
