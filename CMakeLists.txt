cmake_policy(SET CMP0048 NEW)

project(GameEngine)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/CPM.cmake")
  message(STATUS "Downloading CPM.cmake")
  file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v0.27.5/CPM.cmake
    "${CMAKE_BINARY_DIR}/CPM.cmake"
    EXPECTED_HASH SHA256=f858e68c350c21c44497e56b8291c9e679bca3cfbbd96b3240ba5fc69d6ada84)
endif()
include(${CMAKE_BINARY_DIR}/CPM.cmake)

add_subdirectory(src)
add_subdirectory(example/Demo)
add_subdirectory(test)
