cmake_minimum_required(VERSION 3.16)

project(
  nbtview
  VERSION 0.1.0
  DESCRIPTION "A C++ library to read and write data in the NBT format"
  HOMEPAGE_URL "https://github.com/maspitz/nbtview"
  LANGUAGES CXX
)

option(BUILD_TESTS "Build the tests" $(BUILD_TESTS))
option(BUILD_BENCHMARKS "Build the benchmark executables" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 23)

if(CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
  add_definitions(-DTARGET_BIG_ENDIAN)
else()
  add_definitions(-DTARGET_LITTLE_ENDIAN)
endif()


add_subdirectory(nbtview)
add_subdirectory(apps)

if(BUILD_BENCHMARKS)
  find_package(benchmark REQUIRED)
  add_executable(bench_utils benchmarks/bench_utils.cpp)
  target_include_directories(bench_utils PUBLIC "${PROJECT_SOURCE_DIR}/nbtview")
  target_link_libraries(bench_utils PRIVATE benchmark::benchmark)
endif(BUILD_BENCHMARKS)

find_package(Doxygen)

if(DOXYGEN_FOUND)

#  set(DOXYGEN_SOURCE_BROWSER YES)
#  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_EXTRACT_ALL NO)
  set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
  set(DOXYGEN_GENERATE_XML YES)
  set(DOXYGEN_XML_OUTPUT "xml")
  set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen")

  # this target will only be built if specifically requested:
  # run "make docs" to generate the documentation
  doxygen_add_docs(
    docs
    "${PROJECT_SOURCE_DIR}/nbtview"
    COMMENT "Generate documentation for nbtview."
  )
endif(DOXYGEN_FOUND)

if (BUILD_TESTS)
  include(CTest)
  enable_testing()

  function(add_nbtview_test TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE nbtview)
    target_include_directories(${TARGET_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
    add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
  endfunction()

  add_nbtview_test(test_BinaryReader test/test_BinaryReader.cpp)
  add_nbtview_test(test_BinaryWriter test/test_BinaryWriter.cpp)
  add_nbtview_test(test_Serializer test/test_Serializer.cpp)
  add_nbtview_test(test_BinaryDeserializer test/test_BinaryDeserializer.cpp)
  add_nbtview_test(test_nbtview test/test_nbtview.cpp)
  add_nbtview_test(test_bigtest test/test_bigtest.cpp)
  add_nbtview_test(test_Region test/test_Region.cpp)
  add_nbtview_test(test_Chunks test/test_Chunks.cpp)

  # provision test data
  file(COPY test/test_data DESTINATION ${PROJECT_BINARY_DIR}/)
endif (BUILD_TESTS)
