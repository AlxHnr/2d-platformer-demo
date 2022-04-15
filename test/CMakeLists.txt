CPMAddPackage(NAME doctest VERSION 2.4.8
  URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.8.tar.gz
  URL_HASH SHA256=f52763630aa17bd9772b54e14b6cdd632c87adf0169455a86a49bd94abf2cd83)

CPMAddPackage(NAME trompeloeil VERSION 39
  URL https://github.com/rollbear/trompeloeil/archive/v39.tar.gz
  URL_HASH SHA256=10506e48abd605740bc9ed43e34059f5068bc80af14476bd129a3ed3b54d522f)

add_executable(Test
  ConvexBoundingPolygon.cpp
  Geometry.cpp
  Main.cpp
  Physics/Integrator.cpp
)
target_link_libraries(Test GameEngine doctest trompeloeil)
add_custom_target(test COMMAND Test)

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
