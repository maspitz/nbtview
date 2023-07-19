find_package(ZLIB REQUIRED)

add_library(nbtview STATIC nbtview.cpp BinaryDeserializer.cpp Tag.cpp zlib_utils.cpp Stringifier.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nbtview ZLIB::ZLIB)

install(TARGETS nbtview DESTINATION lib)

install(FILES nbtview.hpp Tag.hpp Serializer.hpp Stringifier.hpp DESTINATION include)
