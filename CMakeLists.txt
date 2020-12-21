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

CPMAddPackage(NAME glm VERSION 0.9.9.8
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e)

CPMAddPackage(NAME SDL2 VERSION 2.0.12
  URL https://www.libsdl.org/release/SDL2-2.0.12.tar.gz
  URL_HASH SHA256=349268f695c02efbc9b9148a70b85e58cefbbf704abd3e91be654db7f1e2c863)

CPMAddPackage(NAME span-lite VERSION 0.9.0
  GITHUB_REPOSITORY martinmoene/span-lite
  GIT_TAG f894171e9a5b9c3a8b63a508ad86f5ca085c4954)

add_executable(GameEngine
  src/ConvexBoundingPolygon.cpp
  src/Main.cpp
  src/Math.cpp
  src/PolygonDemo.cpp
  src/PolygonEdge.cpp
  src/SDL2/Error.cpp
)
if(GAME_ENGINE_CXX_FLAGS)
  target_compile_options(GameEngine PRIVATE "SHELL:${GAME_ENGINE_CXX_FLAGS}")
  target_link_options(GameEngine PRIVATE "SHELL:${GAME_ENGINE_CXX_FLAGS}")
endif()
target_link_libraries(GameEngine PRIVATE glm SDL2 span-lite)

add_custom_target(run COMMAND GameEngine)
