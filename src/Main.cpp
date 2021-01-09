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
const size_t screen_width = 1280;
const size_t screen_height = 800;

/** @return Pair containing [window, renderer]. */
auto makeWindowAndRenderer() {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  if (SDL_CreateWindowAndRenderer(screen_width, screen_height, 0, &window, &renderer) != 0) {
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
  auto duration_of_last_frame = 0us;
  const auto *buttons = SDL_GetKeyboardState(nullptr);

  Game game{screen_width, screen_height};

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
          game = Game{screen_width, screen_height};
        }
      } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          game.addStaticBox({event.button.x, event.button.y});
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          game.addDynamicBox({event.button.x, event.button.y});
        }
      } else if (event.type == SDL_MOUSEWHEEL) {
        if (buttons[SDL_SCANCODE_LCTRL]) {
          game.rotateCamera(event.wheel.y * 0.05);
        } else {
          game.scaleCamera(event.wheel.y * 0.05);
        }
      }
    }

    if (buttons[SDL_SCANCODE_LEFT]) {
      game.getGameCharacter().accelerate(HorizontalDirection::Left);
    } else if (buttons[SDL_SCANCODE_RIGHT]) {
      game.getGameCharacter().accelerate(HorizontalDirection::Right);
    } else {
      game.getGameCharacter().accelerate(HorizontalDirection::None);
    }

    game.integratePhysics(duration_of_last_frame);

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());
    game.render(renderer.get());
    SDL_RenderPresent(renderer.get());

    const auto frame_duration = std::chrono::steady_clock::now() - frame_start_time;
    std::this_thread::sleep_for(std::chrono::microseconds{1s} / 60 - frame_duration);

    duration_of_last_frame = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - frame_start_time);
  }
}
