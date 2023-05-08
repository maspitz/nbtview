add_library(nbtview STATIC nbtview.cpp nbtview.hpp)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS nbtview DESTINATION lib)

install(FILES nbtview.hpp DESTINATION include)


