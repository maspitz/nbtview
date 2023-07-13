cmake_minimum_required(VERSION 3.16)

project(
  nbtview
  VERSION 0.1.0
  DESCRIPTION "A C++ library to read and write data in the NBT format"
  HOMEPAGE_URL "https://github.com/maspitz/nbtview"
  LANGUAGES CXX
)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
  add_definitions(-DTARGET_BIG_ENDIAN)
else()
  add_definitions(-DTARGET_LITTLE_ENDIAN)
endif()

include(CTest)

enable_testing() 
add_subdirectory(nbtview)
#add_subdirectory(doc)
add_subdirectory(apps)

# look for Doxygen package
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

add_executable(test_BinaryScanner test/test_BinaryScanner.cpp)
target_link_libraries(test_BinaryScanner nbtview)
target_include_directories(test_BinaryScanner PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_BinaryScanner test_BinaryScanner)

add_executable(test_BinaryWriter test/test_BinaryWriter.cpp)
target_link_libraries(test_BinaryWriter nbtview)
target_include_directories(test_BinaryWriter PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_BinaryWriter test_BinaryWriter)

add_executable(test_Serializer test/test_Serializer.cpp)
target_link_libraries(test_Serializer nbtview)
target_include_directories(test_Serializer PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_Serializer test_Serializer)

add_executable(test_BinaryDeserializer test/test_BinaryDeserializer.cpp)
target_link_libraries(test_BinaryDeserializer nbtview)
target_include_directories(test_BinaryDeserializer PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_BinaryDeserializer test_BinaryDeserializer)

add_executable(test_nbtview test/test_nbtview.cpp)
target_link_libraries(test_nbtview nbtview)
target_include_directories(test_nbtview PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_nbtview test_nbtview)

add_executable(test_bigtest test/test_bigtest.cpp)
target_link_libraries(test_bigtest nbtview)
target_include_directories(test_bigtest PRIVATE "${PROJECT_SOURCE_DIR}/doctest")
add_test(Test_bigtest test_bigtest)

# provision test data
file(COPY test/test_data DESTINATION ${CMAKE_BINARY_DIR}/)
