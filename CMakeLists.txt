cmake_minimum_required(VERSION 3.16)

project(
  nbtview
  VERSION 0.1.0
  DESCRIPTION "A C++ library for fast, read-only access to data in the NBT format"
  HOMEPAGE_URL "https://github.com/maspitz/nbtview"
  LANGUAGES CXX
)

#set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 20)

include(CTest)

enable_testing() 
add_subdirectory(nbtview)
#add_subdirectory(doc)
add_subdirectory(apps)

add_executable(test_BinaryScanner test/test_BinaryScanner.cpp)
target_link_libraries(test_BinaryScanner nbtview)
target_include_directories(test_BinaryScanner PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_BinaryScanner test_BinaryScanner)

add_executable(test_nbtview test/test_nbtview.cpp)
target_link_libraries(test_nbtview nbtview)
target_include_directories(test_nbtview PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_nbtview test_nbtview)
