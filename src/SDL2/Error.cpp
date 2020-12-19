/** @file
 * Implements simple wrappers around SDL2 error functions.
 */

#include "Error.hpp"
#include <SDL_error.h>
#include <system_error>

namespace GameEngine::SDL2 {
void throwRuntimeError(std::string_view message) {
  throw std::runtime_error{
      std::string{"Error: "}.append(message).append(": ").append(SDL_GetError())};
}
} // namespace GameEngine::SDL2
