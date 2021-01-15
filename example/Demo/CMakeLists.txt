add_executable(Demo
  Game.cpp
  Main.cpp
)
target_link_libraries(Demo GameEngine)
add_custom_target(run COMMAND Demo)
