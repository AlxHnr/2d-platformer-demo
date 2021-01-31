/** @file
 * Implements simple wrappers around SDL2 error functions.
 */

#include "GameEngine/SDL2/Error.hpp"
#include <SDL_error.h>

namespace GameEngine::SDL2 {
std::runtime_error makeRuntimeError(std::string_view message) {
  return std::runtime_error{
      std::string{"Error: "}.append(message).append(": ").append(SDL_GetError())};
}
} // namespace GameEngine::SDL2
