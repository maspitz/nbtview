# nbtview
A C++ library to read and write data in the NBT format used by Minecraft.

## Usage

**Output an NBT file in a string form.**

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
mkdir build
cd build
cmake ..
make
make install
```

## Documentation

A `docs` target is provided, which requires doxygen.

In the `build` directory, run `make docs`.

Documentation is emitted into `build/doxygen`.

## Testing

Test executables are part of the default build target.  They can be run individually or via `ctest`.
