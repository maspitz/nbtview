// nbtshow.cpp

#include <iostream>
#include <fstream>
#include <vector>

#include "nbtview.hpp"

namespace nbt = nbtview;

int main() {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return -1;
	}
    std::string filename(argv[1]);

    // Get the file size
    std::ifstream file(filename, std::ios::binary | std::ios::in | std::ios::ate);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the entire file into a vector of bytes
    std::vector<std::byte> bytes(size);
    file.read(reinterpret_cast<char*>(bytes.data()), size);


    return 0;
}
