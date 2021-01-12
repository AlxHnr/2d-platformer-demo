/** @file
 * Contains a class which implements typical jump-and-run physics.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_JUMP_AND_RUN_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICS_JUMP_AND_RUN_OBJECT_HPP

#include "Physics/DynamicObject.hpp"

namespace GameEngine::Physics {
/** Physical object which can accelerate and run up slopes, move in the air and do wall jumps.
 * The behaviour depends on the objects properties. */
class JumpAndRunObject : public DynamicObject {
public:
  /** Construct an object implementing conventional jump-and-run mechanics.
   *
   * @param vertices Convex polygon with zero or more points representing the borders of the object
   * used for collision detection.
   */
  JumpAndRunObject(std::initializer_list<glm::vec2> vertices);

  void update() override;

  /** Try to do a jump, walljump or airjump depending on the situation. Calls to this function will
   * be buffered for a very short time to remove the need for pixel-perfect user input. */
  void jump();

  /** @param direction Optional horizontal acceleration to be applied to the object. If no direction
   * was given, the object will slow down and stop. The slope of the ground will be considered. */
  void run(std::optional<HorizontalDirection> direction);

private:
  /** Length of a jumps velocity vector. */
  float jump_power = 0.475;

  /** Will be applied to the object on each tick while running. Application of this value depends on
   * the slope of the ground and whether the object is in the air or not. */
  float horizontal_acceleration = 0.05;

  /** Speed limit at which the acceleration stops. */
  float horizontal_speed_max = 0.35;

  /** Counts each update() call. Set to 1000 ticks in the future to allow tick_of_jump_request to be
   * (re)set to 0. */
  uint32_t current_tick = 1000;

  /** Tick at which jump() was called. Allows pressing the jump button slightly before the ground or
   * wall is touched. */
  uint32_t tick_of_jump_request = 0;

  std::optional<HorizontalDirection> acceleration_direction = std::nullopt;
};
} // namespace GameEngine::Physics

#endif
