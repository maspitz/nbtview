find_package(ZLIB REQUIRED)

add_library(nbtview STATIC nbtview.cpp BinaryDeserializer.cpp NbtReader.cpp tag_utilities.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nbtview ZLIB::ZLIB)

install(TARGETS nbtview DESTINATION lib)

install(FILES nbtview.hpp Tag.hpp Compound.hpp List.hpp Serializer.hpp NbtReader.hpp tag_utilities.hpp DESTINATION include)
