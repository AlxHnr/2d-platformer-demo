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

  bool program_running = true;
  auto [window, renderer] = makeWindowAndRenderer();
  auto last_print_time = std::chrono::steady_clock::now();
  auto time_delta = 0us;

  while (program_running) {
    const auto frame_start_time = std::chrono::steady_clock::now();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        program_running = false;
        break;
      }
    }

    SDL_RenderClear(renderer.get());
    SDL_RenderPresent(renderer.get());

    const auto frame_duration = std::chrono::steady_clock::now() - frame_start_time;
    std::this_thread::sleep_for(std::chrono::microseconds{1s} / 60 - frame_duration);
    time_delta = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - frame_start_time);

    if (std::chrono::steady_clock::now() - last_print_time > 1s) {
      std::cout << "Frame duration: "
                << std::chrono::duration_cast<std::chrono::microseconds>(frame_duration).count()
                << ", time delta:"
                << std::chrono::duration_cast<std::chrono::microseconds>(time_delta).count()
                << std::endl;
      last_print_time = std::chrono::steady_clock::now();
    }
  }
}
