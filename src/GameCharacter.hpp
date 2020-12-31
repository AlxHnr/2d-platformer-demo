/** @file
 * Contains an interactive character which can run, jump and fall.
 */

#ifndef GAME_ENGINE_SRC_GAME_CHARACTER_HPP
#define GAME_ENGINE_SRC_GAME_CHARACTER_HPP

#include "ConvexBoundingPolygon.hpp"
#include <glm/vec2.hpp>

namespace GameEngine {
/** Interactive character which can run, jump and fall. */
struct GameCharacter {
  /** Construct a Polygon from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  GameCharacter(std::initializer_list<glm::vec2> vertices);

  const glm::vec2 &getVelocity() const;
  void update();

  bool handleCollisionWith(GameCharacter &other, const glm::vec2 &displacement_vector);

  void jump();
  bool jumpScheduled() const;
  bool sticksToWall() const;

  enum class VerticalAcceleration { None, Left, Right };
  void accelerate(VerticalAcceleration direction);

  ConvexBoundingPolygon bounding_polygon;

  enum class State { Falling, TouchingWall, TouchingGround } state{};

  /** Speed * direction of this object. */
  glm::vec2 velocity{0.0f, 0.0f};

  uint32_t current_tick = 1000;
  uint32_t tick_of_jump_request = 0;

  /** Contains the direction from the colliding wall. */
  bool wall_jump_to_right = false;

  VerticalAcceleration acceleration_direction = VerticalAcceleration::None;

  /** Right direction perpendicular to the slope of the floor. Required for running on slopes. */
  glm::vec2 right_direction{1, 0};
  bool touching_ground = false;
  bool touching_wall = false;
};
} // namespace GameEngine

#endif
