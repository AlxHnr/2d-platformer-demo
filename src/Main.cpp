/** @file
 * Main playground for testing code.
 */

#include "SDL2/Error.hpp"
#include "SDL2/UniquePointer.hpp"
#include <SDL.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>
#include <utility>

using namespace GameEngine;
using namespace std::chrono_literals;

namespace {
/** @return Pair containing [window, renderer]. */
auto makeWindowAndRenderer() {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  if (SDL_CreateWindowAndRenderer(1280, 800, 0, &window, &renderer) != 0) {
    SDL2::throwRuntimeError("Failed to create window and renderer");
  }
  return std::pair{SDL2::wrapPointer(window), SDL2::wrapPointer(renderer)};
}
} // namespace

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

  bool program_running = true;
  while (program_running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        program_running = false;
        break;
      }
    }

    SDL_RenderClear(renderer.get());
    SDL_RenderPresent(renderer.get());
    std::this_thread::sleep_for(16ms);
  }
}
