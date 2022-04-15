CPMAddPackage(NAME glm VERSION 0.9.9.8
  URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
  URL_HASH SHA256=37e2a3d62ea3322e43593c34bae29f57e3e251ea89f4067506c94043769ade4c)

CPMAddPackage(NAME SDL2 VERSION 2.0.20
  URL https://libsdl.org/release/SDL2-2.0.20.tar.gz
  URL_HASH SHA256=c56aba1d7b5b0e7e999e4a7698c70b63a3394ff9704b5f6e1c57e0c16f04dd06)

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
target_link_libraries(GameEngine PUBLIC glm SDL2)
target_include_directories(GameEngine PUBLIC ../include)
