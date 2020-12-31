/** @file
 * Main playground for testing code.
 */

#include "Game.hpp"
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
  auto time_delta = 0us;
  const auto *buttons = SDL_GetKeyboardState(nullptr);
  Game game;

  while (program_running) {
    const auto frame_start_time = std::chrono::steady_clock::now();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        program_running = false;
        break;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_UP) {
          game.getGameCharacter().jump();
        } else if (event.key.keysym.sym == SDLK_r) {
          game = Game{};
        }
      }
    }

    if (buttons[SDL_SCANCODE_LEFT]) {
      game.getGameCharacter().accelerate(GameCharacter::VerticalAcceleration::Left);
    } else if (buttons[SDL_SCANCODE_RIGHT]) {
      game.getGameCharacter().accelerate(GameCharacter::VerticalAcceleration::Right);
    } else {
      game.getGameCharacter().accelerate(GameCharacter::VerticalAcceleration::None);
    }

    game.integratePhysics();

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());
    game.render(renderer.get());
    SDL_RenderPresent(renderer.get());

    const auto frame_duration = std::chrono::steady_clock::now() - frame_start_time;
    std::this_thread::sleep_for(std::chrono::microseconds{1s} / 60 - frame_duration);
    time_delta = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - frame_start_time);
  }
}
