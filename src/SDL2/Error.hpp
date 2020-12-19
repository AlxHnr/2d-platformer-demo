/** @file
 * Simple wrappers around SDL2 error functions.
 */

#ifndef GAME_ENGINE_SRC_SDL2_ERROR_HPP
#define GAME_ENGINE_SRC_SDL2_ERROR_HPP

#include <string_view>

namespace GameEngine::SDL2 {
/** Throw an std::runtime_error with the current SDL2 error description.
 *
 * @param message Describes the reason for throwing.
 */
void throwRuntimeError(std::string_view message);
} // namespace GameEngine::SDL2

#endif
