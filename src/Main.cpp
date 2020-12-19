/** @file
 * Main playground for testing code.
 */

#include <SDL.h>
#include <iostream>
#include <memory>
#include <string_view>

#include "SDL2/Error.hpp"
#include "SDL2/UniquePointer.hpp"

using namespace GameEngine;

int main() {
  struct Context {
    Context() {
      if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL2::throwRuntimeError("Failed to initialize SDL2");
      }
    }

    ~Context() { SDL_Quit(); }
  } context{};

  SDL_Window* raw_window = nullptr;
  SDL_Renderer* raw_renderer = nullptr;
  if (SDL_CreateWindowAndRenderer(1280, 800, 0, &raw_window, &raw_renderer) != 0) {
    SDL2::throwRuntimeError("Failed to create window and renderer");
  }
  auto window = SDL2::wrapPointer(raw_window);
  auto renderer = SDL2::wrapPointer(raw_renderer);

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
