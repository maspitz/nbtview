find_package(ZLIB REQUIRED)

add_executable(nbtshow
    nbtshow.cpp
)

target_link_libraries(nbtshow nbtview ZLIB::ZLIB)
