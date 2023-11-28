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

  add_executable(bench_chunks benchmarks/bench_chunks.cpp)
  target_include_directories(bench_chunks PUBLIC "${PROJECT_SOURCE_DIR}/nbtview")
  target_link_libraries(bench_chunks PRIVATE benchmark::benchmark nbtview)
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

  find_package(GTest REQUIRED)

  add_executable(tests test/test_main.cpp test/test_BinaryWriter.cpp test/test_BinaryReader.cpp test/test_Chunks.cpp test/test_BinaryDeserializer.cpp test/test_nbtview.cpp test/test_Region.cpp test/test_Serializer.cpp test/test_bigtest.cpp)
  target_link_libraries(tests PRIVATE nbtview GTest::GTest)
  add_test(NAME tests COMMAND tests)

  # provision test data
  file(COPY test/test_data DESTINATION ${PROJECT_BINARY_DIR}/)
endif (BUILD_TESTS)
