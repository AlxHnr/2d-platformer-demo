/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "GameCharacter.hpp"
#include <SDL_render.h>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game();

  GameCharacter &getGameCharacter();
  void integratePhysics();
  void render(SDL_Renderer *renderer) const;

private:
  std::vector<GameCharacter> objects;
};
} // namespace GameEngine

#endif
