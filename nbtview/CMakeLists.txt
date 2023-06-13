find_package(ZLIB REQUIRED)

add_library(nbtview STATIC nbtview.cpp Compound.cpp List.cpp BinaryScanner.cpp NbtReader.cpp Tag.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nbtview ZLIB::ZLIB)

install(TARGETS nbtview DESTINATION lib)

install(FILES nbtview.hpp Tag.hpp Compound.hpp List.hpp BinaryScanner.hpp NbtReader.hpp DESTINATION include)

