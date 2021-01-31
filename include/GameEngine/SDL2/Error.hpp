/** @file
 * Simple wrappers around SDL2 error functions.
 */

#ifndef GAME_ENGINE_INCLUDE_GAME_ENGINE_SDL2_ERROR_HPP
#define GAME_ENGINE_INCLUDE_GAME_ENGINE_SDL2_ERROR_HPP

#include <string_view>
#include <system_error>

namespace GameEngine::SDL2 {
/** Construct an std::runtime_error with the current SDL2 error description.
 *
 * @param message Describes the reason for throwing.
 *
 * @return Runtime error containing the given reason and the current SDL2 error description.
 */
std::runtime_error makeRuntimeError(std::string_view message);
} // namespace GameEngine::SDL2

#endif
