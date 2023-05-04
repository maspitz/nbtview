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

set(CMAKE_CXX_STANDARD 20)

add_library(libnbtview
	source/nbtview.cpp
)
target_include_directories(libnbtview PUBLIC include)

add_executable(nbtshow
    apps/nbtshow.cpp
)

target_link_libraries(nbtshow libnbtview)
target_include_directories(nbtshow PRIVATE include)
