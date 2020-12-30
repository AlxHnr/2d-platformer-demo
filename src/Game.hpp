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
  bool jumpScheduled() const;
  bool collidesWithFloor() const;
  bool collidesWithWall() const;

  PolygonObject game_character;
  std::vector<PolygonObject> objects;

  uint64_t current_tick = 1000;
  uint64_t tick_of_jump_request = 0;
  uint64_t tick_of_last_floor_collision = 0;
  uint64_t tick_of_last_wall_collision = 0;
  bool wall_jump_to_right = false;

  glm::vec2 right_direction{0, 0};
  Acceleration acceleration_direction = Acceleration::None;
};
} // namespace GameEngine

#endif
