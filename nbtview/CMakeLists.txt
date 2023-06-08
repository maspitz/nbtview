add_library(nbtview STATIC nbtview.cpp Compound.cpp List.cpp BinaryScanner.cpp Tag.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS nbtview DESTINATION lib)

install(FILES nbtview.hpp Tag.hpp Compound.hpp List.hpp BinaryScanner.hpp DESTINATION include)
