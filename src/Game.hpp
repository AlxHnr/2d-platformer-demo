/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "PolygonObject.hpp"
#include <SDL_render.h>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game();
  void integratePhysics();
  void render(SDL_Renderer *renderer) const;

private:
  PolygonObject game_character;
  std::vector<PolygonObject> objects;
};
} // namespace GameEngine

#endif
