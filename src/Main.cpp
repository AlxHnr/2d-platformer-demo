/** @file
 * Main playground for testing code.
 */

#include <SDL.h>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

#include "SDL2/Error.hpp"
#include "SDL2/UniquePointer.hpp"

using namespace GameEngine;

namespace {
/** @return Pair containing [window, renderer]. */
auto makeWindowAndRenderer() {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  if (SDL_CreateWindowAndRenderer(1280, 800, 0, &window, &renderer) != 0) {
    SDL2::throwRuntimeError("Failed to create window and renderer");
  }
  return std::pair{SDL2::wrapPointer(window), SDL2::wrapPointer(renderer)};
}
}  // namespace

int main() {
  struct Context {
    Context() {
      if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL2::throwRuntimeError("Failed to initialize SDL2");
      }
    }

    ~Context() { SDL_Quit(); }
  } context{};

  auto [window, renderer] = makeWindowAndRenderer();

  SDL_RenderClear(renderer.get());
  SDL_RenderPresent(renderer.get());

  bool program_running = true;
  while (program_running) {
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT) {
        program_running = false;
        break;
      }
    }
  }

  std::cout << "Hello World" << std::endl;
}
