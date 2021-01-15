/** @file
 * Helper functions for wrapping SDL2 types in std::unique_ptr.
 */

#ifndef GAME_ENGINE_INCLUDE_GAME_ENGINE_SDL2_UNIQUE_POINTER_HPP
#define GAME_ENGINE_INCLUDE_GAME_ENGINE_SDL2_UNIQUE_POINTER_HPP

#include <SDL_render.h>
#include <SDL_video.h>
#include <memory>

namespace GameEngine::SDL2 {
struct Deleter {
  void operator()(SDL_Window *window) { SDL_DestroyWindow(window); };
  void operator()(SDL_Renderer *renderer) { SDL_DestroyRenderer(renderer); };
};

template <typename T> using UniquePointer = std::unique_ptr<T, Deleter>;

/** Wrap the given pointer in a unique pointer using the matching SDL function for releasing. */
template <typename T> UniquePointer<T> wrapPointer(T *ptr) { return UniquePointer<T>{ptr}; }
} // namespace GameEngine::SDL2

#endif
