#include <iostream>

#include "nbtview/nbtview.hpp"

namespace nbt = nbtview;

int main() {
    std::cout << "nbtshow: Hello!\n";
    std::cout << nbt::hello_string() << std::endl;
}
