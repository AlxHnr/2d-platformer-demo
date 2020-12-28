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

CPMAddPackage(NAME SDL2 VERSION 2.0.14
  URL https://www.libsdl.org/release/SDL2-2.0.14.tar.gz
  URL_HASH SHA256=d8215b571a581be1332d2106f8036fcb03d12a70bae01e20f424976d275432bc)

CPMAddPackage(NAME span-lite VERSION 0.9.0
  GITHUB_REPOSITORY martinmoene/span-lite
  GIT_TAG f894171e9a5b9c3a8b63a508ad86f5ca085c4954)

CPMAddPackage(NAME doctest VERSION 2.4.3
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 352589e5c3bcce53bcbc05da2f2611d614348f3d)

add_library(GameEngine
  src/Geometry.cpp
  src/SDL2/Error.cpp
)
if(GAME_ENGINE_CXX_FLAGS)
  target_compile_options(GameEngine PUBLIC "SHELL:${GAME_ENGINE_CXX_FLAGS}")
  target_link_options(GameEngine PUBLIC "SHELL:${GAME_ENGINE_CXX_FLAGS}")
endif()
target_link_libraries(GameEngine PUBLIC glm SDL2 span-lite)
target_include_directories(GameEngine PUBLIC src)

add_executable(Test
  test/Main.cpp
  test/Geometry.cpp
)
target_link_libraries(Test GameEngine doctest)
add_custom_target(test COMMAND Test)

add_executable(Demo
  src/Game.cpp
  src/Main.cpp
  src/PolygonObject.cpp
)
target_link_libraries(Demo GameEngine)
add_custom_target(run COMMAND Demo)

find_program(GCOVR gcovr)
if(GCOVR)
  add_custom_target(coverage
    COMMAND "${GCOVR}" --delete "${CMAKE_BINARY_DIR}"
    COMMAND Test
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/coverage"
    COMMAND "${GCOVR}" --sort-percentage --filter "${PROJECT_SOURCE_DIR}/src/\\*"
      --html-details "${CMAKE_BINARY_DIR}/coverage/index.html")
else()
  add_custom_target(coverage
    COMMAND "${CMAKE_COMMAND}" -E echo "gcovr command not found"
    COMMAND "${CMAKE_COMMAND}" -E false)
endif()
