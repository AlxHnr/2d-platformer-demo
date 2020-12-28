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

  void scheduleJump();

  enum class Direction { Left, Right };
  void accelerateCharacter(Direction direction);

  void integratePhysics();
  void render(SDL_Renderer *renderer) const;

private:
  bool jump_scheduled{false};
  PolygonObject game_character;
  std::vector<PolygonObject> objects;

  glm::vec2 last_displacement_vector;
  bool is_touching_floor = false;
};
} // namespace GameEngine

#endif
