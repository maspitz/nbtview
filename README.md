# nbtview
A C++ library to read and write data in the NBT format used by Minecraft.

## Usage

**Example: Output an NBT file in a string form.**

```cpp
    std::ifstream infile("my_file.nbt");

    auto [root_name, root_tag] = nbt::read_binary(infile);
    std::cout << "root_name: " << root_name << std::endl;
    std::cout << "root_tag: " << root_tag << std::endl;
```

See `test/test_nbtview.cpp` for more example usage.

## Building

**Requirements**

zlib is required.  In Fedora (for example) this can be satisfied via `dnf install zlib-devel`

**Compiling**

``` bash
git clone https://github.com/maspitz/nbtview
cd nbtview
cmake -S . -B build
cmake --build build -- -j$(nproc)
```

**Documentation**

Assumes `doxygen` is installed:

``` bash
cmake --build build --target docs
```

Documentation is emitted into `build/doxygen`.

**Testing**

``` bash
cmake --build build --target test
```

Or run the test executables individually, e.g.:
`build/test_bigtest`
