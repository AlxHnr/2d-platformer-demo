/** @file
 * Contains a class which implements typical jump-and-run physics.
 */

#ifndef GAME_ENGINE_INCLUDE_GAME_ENGINE_PHYSICS_JUMP_AND_RUN_OBJECT_HPP
#define GAME_ENGINE_INCLUDE_GAME_ENGINE_PHYSICS_JUMP_AND_RUN_OBJECT_HPP

#include "GameEngine/HorizontalDirection.hpp"
#include "GameEngine/Physics/DynamicObject.hpp"
#include <chrono>

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

  /** @return Positive length of each jumps velocity vector. */
  float getJumpPower() const;

  /** @param jump_power Value containing the new length of a jumps velocity vector. If negative,
   * will be set to zero. */
  void setJumpPower(float jump_power);

  /** @return True if the object can jump from a wall during a collision. */
  bool getWalljumpEnabled() const;

  /** Enable or disable walljumping.
   *
   * @param enabled True if the object can jump from a wall during a collision.
   */
  void setWalljumpEnabled(bool enabled);

  /** @return Friction value between 0 and 1, applied while the object slides down a wall. */
  float getWallGrip() const;

  /** @param wall_grip Friction value between 0 and 1, applied while the object slides down a wall.
   * Will be clamped to the expected range. */
  void setWallGrip(float wall_grip);

  /** @return Amount of jumps the object can perform in the air before touching the ground again.
   * Zero means airjumping is disabled. */
  uint16_t getAirjumpsMax() const;

  /** @param airjumps_max Amount of jumps the object can perform in the air before touching the
   * ground again. Zero means airjumping is disabled. */
  void setAirjumpsMax(uint16_t airjumps_max);

  /** @return Positive value which will be applied to the object on each tick while running.
   * Application of this value depends on the slope of the ground and whether the object is in the
   * air or not. */
  float getHorizontalAcceleration() const;

  /** @param horizontal_acceleration Value which will be applied to the object on each tick
   * while running. If negative, will be set to zero. Application of this value depends on the slope
   * of the ground and whether the object is in the air or not. */
  void setHorizontalAcceleration(float horizontal_acceleration);

  /** @return Positive speed limit at which the acceleration stops. */
  float getHorizontalSpeedMax() const;

  /** @param horizontal_speed_max Speed limit at which the acceleration stops. If negative, will be
   * set to zero. */
  void setHorizontalSpeedMax(float horizontal_speed_max);

  /** @return Friction value between 0 and 1, applied when the object touches the ground and stops
   * running. */
  float getGroundGrip() const;

  /** @param ground_grip Friction value between 0 and 1, applied when the object touches the ground
   * and stops running. Will be clamped to the expected range. */
  void setGroundGrip(float ground_grip);

private:
  /** Length of a jumps velocity vector. */
  float jump_power = 0.475;

  /** True if the object can jump from a wall during a collision. */
  bool walljump_enabled = true;

  /** Friction applied to the object while sliding down a wall. */
  float wall_grip = 0.03;

  /** Maximal limit of jumps the object can perform in the air before touching the ground again. */
  uint16_t airjumps_max = 0;

  /** Amount of airjumps the object has left before touching the ground again. */
  uint16_t airjumps_remaining = 0;

  /** Will be applied to the object on each tick while running. Application of this value depends on
   * the slope of the ground and whether the object is in the air or not. */
  float horizontal_acceleration = 0.05;

  /** Speed limit at which the acceleration stops. */
  float horizontal_speed_max = 0.35;

  /** Friction of the floor applied when the object stops running. */
  float ground_grip = 0.05;

  /** Timepoint at which jump() was called. Allows pressing the jump button slightly before the
   * ground or wall is touched. */
  std::chrono::steady_clock::time_point time_of_jump_request{};

  std::optional<HorizontalDirection> acceleration_direction = std::nullopt;
};
} // namespace GameEngine::Physics

#endif
