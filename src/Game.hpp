/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "PhysicalObject.hpp"
#include <SDL_render.h>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game();

  PhysicalObject &getGameCharacter();
  void integratePhysics();
  void render(SDL_Renderer *renderer) const;

private:
  std::vector<PhysicalObject> objects;
};
} // namespace GameEngine

#endif
