cmake_policy(SET CMP0048 NEW)

project(GameEngine)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/CPM.cmake")
  message(STATUS "Downloading CPM.cmake")
  file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.35.0/CPM.cmake
    "${CMAKE_BINARY_DIR}/CPM.cmake"
    EXPECTED_HASH SHA256=6c3015e0359c99994f65d248c8541a9cb02e03419d46961ce7d4e38b50d87635)
endif()
include(${CMAKE_BINARY_DIR}/CPM.cmake)

add_subdirectory(src)
add_subdirectory(example/Demo)
add_subdirectory(test)
