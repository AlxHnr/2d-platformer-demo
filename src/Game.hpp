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

  enum class Acceleration { None, Left, Right };
  void accelerate(Acceleration direction);

  void integratePhysics();
  void render(SDL_Renderer *renderer) const;

private:
  bool isTouchingFloor() const;

  bool jump_scheduled{false};
  PolygonObject game_character;
  std::vector<PolygonObject> objects;

  size_t frames_since_floor_was_touched = 0;

  glm::vec2 right_direction{0, 0};
  Acceleration acceleration_direction = Acceleration::None;
};
} // namespace GameEngine

#endif
