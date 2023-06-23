# nbtview
A C++ library for fast, read-only access to data in the NBT format (as used by Minecraft)

## Building

zlib is required.  In Fedora (for example) this can be satisfied via `dnf install zlib-devel`

C++20 is used.

``` bash
mkdir build
cd build
cmake ..
make
```

## Usage

**Output an NBT file in a string form.**

```cpp
auto root_tag = nbt::NbtReader::read_from_file(filename);
std::cout << "root_tag: " << root_tag.to_string() << std::endl;
```

See `test/test_nbtview.cpp` for more example usage.
