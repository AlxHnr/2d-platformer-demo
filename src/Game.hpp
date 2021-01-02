/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "Physics/DynamicObject.hpp"
#include "Physics/Object.hpp"
#include <SDL_render.h>
#include <memory>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game();

  Physics::DynamicObject &getGameCharacter();
  const Physics::DynamicObject &getGameCharacter() const;
  void integratePhysics();
  void render(SDL_Renderer *renderer) const;

private:
  std::vector<std::unique_ptr<Physics::Object>> objects;
};
} // namespace GameEngine

#endif
