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

CPMAddPackage(NAME doctest VERSION 2.4.4
  URL https://github.com/onqtam/doctest/archive/2.4.4.tar.gz
  URL_HASH SHA256=3bcb62ad316bf4230873a336fcc6eb6292116568a6e19ab8cdd37a1610773d70)

CPMAddPackage(NAME glm VERSION 0.9.9.8
  URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
  URL_HASH SHA256=37e2a3d62ea3322e43593c34bae29f57e3e251ea89f4067506c94043769ade4c)

CPMAddPackage(NAME SDL2 VERSION 2.0.14
  URL https://www.libsdl.org/release/SDL2-2.0.14.tar.gz
  URL_HASH SHA256=d8215b571a581be1332d2106f8036fcb03d12a70bae01e20f424976d275432bc)

CPMAddPackage(NAME span-lite VERSION 0.9.0
  URL https://github.com/martinmoene/span-lite/archive/v0.9.0.tar.gz
  URL_HASH SHA256=cdb5f86e5f5e679d63700a56de734c44fe22a574a17347d09dbaaef80619af91)

CPMAddPackage(NAME trompeloeil VERSION 39
  URL https://github.com/rollbear/trompeloeil/archive/v39.tar.gz
  URL_HASH SHA256=10506e48abd605740bc9ed43e34059f5068bc80af14476bd129a3ed3b54d522f)

add_library(GameEngine
  src/Camera.cpp
  src/ConvexBoundingPolygon.cpp
  src/Geometry.cpp
  src/Physics/DynamicObject.cpp
  src/Physics/Integrator.cpp
  src/Physics/JumpAndRunObject.cpp
  src/Physics/StaticObject.cpp
  src/SDL2/Error.cpp
)
if(GAME_ENGINE_CXX_FLAGS)
  target_compile_options(GameEngine PUBLIC "SHELL:${GAME_ENGINE_CXX_FLAGS}")
  target_link_options(GameEngine PUBLIC "SHELL:${GAME_ENGINE_CXX_FLAGS}")
endif()
target_link_libraries(GameEngine PUBLIC glm SDL2 span-lite)
target_include_directories(GameEngine PUBLIC src)

add_executable(Test
  test/ConvexBoundingPolygon.cpp
  test/Geometry.cpp
  test/Main.cpp
  test/Physics/Integrator.cpp
)
target_link_libraries(Test GameEngine doctest trompeloeil)
add_custom_target(test COMMAND Test)

add_executable(Demo
  src/Game.cpp
  src/Main.cpp
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
