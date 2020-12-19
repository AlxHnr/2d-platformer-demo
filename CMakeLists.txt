project(GameEngine)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GameEngine
  src/Main.cpp)
if(GAME_ENGINE_CXX_FLAGS)
  target_compile_options(GameEngine PRIVATE "SHELL:${GAME_ENGINE_CXX_FLAGS}")
  target_link_options(GameEngine PRIVATE "SHELL:${GAME_ENGINE_CXX_FLAGS}")
endif()

add_custom_target(run COMMAND GameEngine)
