/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "PolygonObject.hpp"
#include <SDL_render.h>
#include <chrono>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game();
  void handleFrame(SDL_Renderer *renderer, std::chrono::microseconds time_delta);

private:
  std::vector<PolygonObject> objects;
};
} // namespace GameEngine

#endif
