add_library(nbtview STATIC nbtview.cpp List.cpp BinaryScanner.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS nbtview DESTINATION lib)

install(FILES nbtview.hpp List.hpp BinaryScanner.hpp DESTINATION include)
