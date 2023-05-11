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
add_subdirectory(doc)
add_subdirectory(apps)

Include(FetchContent)

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.3.2
)

FetchContent_MakeAvailable(Catch2)
include(Catch)

add_executable(test_nbtview nbtview/test_nbtview.cpp)
#target_include_directories(test_nbtview PUBLIC "${PROJECT_SOURCE_DIR}/nbtview")
catch_discover_tests(test_nbtview)
target_link_libraries(test_nbtview PRIVATE Catch2::Catch2WithMain)
