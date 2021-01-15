CPMAddPackage(NAME glm VERSION 0.9.9.8
  URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
  URL_HASH SHA256=37e2a3d62ea3322e43593c34bae29f57e3e251ea89f4067506c94043769ade4c)

CPMAddPackage(NAME SDL2 VERSION 2.0.14
  URL https://www.libsdl.org/release/SDL2-2.0.14.tar.gz
  URL_HASH SHA256=d8215b571a581be1332d2106f8036fcb03d12a70bae01e20f424976d275432bc)

CPMAddPackage(NAME span-lite VERSION 0.9.0
  URL https://github.com/martinmoene/span-lite/archive/v0.9.0.tar.gz
  URL_HASH SHA256=cdb5f86e5f5e679d63700a56de734c44fe22a574a17347d09dbaaef80619af91)

add_library(GameEngine
  Camera.cpp
  ConvexBoundingPolygon.cpp
  Geometry.cpp
  Physics/DynamicObject.cpp
  Physics/Integrator.cpp
  Physics/JumpAndRunObject.cpp
  Physics/StaticObject.cpp
  SDL2/Error.cpp
)
target_link_libraries(GameEngine PUBLIC glm SDL2 span-lite)
target_include_directories(GameEngine PUBLIC ../include)
