find_package(ZLIB REQUIRED)

add_library(nbtview STATIC nbtview.cpp BinaryDeserializer.cpp Region.cpp zlib_utils.cpp)

target_include_directories(nbtview PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nbtview ZLIB::ZLIB)

install(TARGETS nbtview DESTINATION lib)

install(FILES nbtview.hpp Region.hpp Tag.hpp utils.hpp DESTINATION include)
