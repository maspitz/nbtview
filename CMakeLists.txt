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

add_executable(test_nbtview_detail nbtview/test_nbtview_detail.cpp)
target_include_directories(test_nbtview_detail PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_nbtview_detail test_nbtview_detail)
add_test(Test_nbtview test_nbtview)
