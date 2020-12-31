/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "GameCharacter.hpp"
#include "PhysicalObject.hpp"
#include <SDL_render.h>
#include <memory>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game();

  GameCharacter &getGameCharacter();
  const GameCharacter &getGameCharacter() const;
  void integratePhysics();
  void render(SDL_Renderer *renderer) const;

private:
  std::vector<std::unique_ptr<PhysicalObject>> objects;
};
} // namespace GameEngine

#endif
