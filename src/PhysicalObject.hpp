/** @file
 * Declares an interactive polygon.
 */

#ifndef GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP

#include "ConvexBoundingPolygon.hpp"
#include <glm/vec2.hpp>

namespace GameEngine {
/** Basic convex polygon to which physics and collisions can be applied. */
struct PhysicalObject {
  /** Construct a Polygon from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  PhysicalObject(std::initializer_list<glm::vec2> vertices);

  void jump();
  void update();
  bool jumpScheduled() const;
  bool sticksToWall() const;

  enum class Acceleration { None, Left, Right };
  void accelerate(Acceleration direction);

  ConvexBoundingPolygon bounding_polygon;

  enum class State { Falling, TouchingWall, TouchingGround } state{};

  /** Speed perpendicular to the slope of the current floor. */
  float vertical_velocity = 0;

  /** Speed * direction of this object. */
  glm::vec2 velocity{0.0f, 0.0f};

  uint32_t current_tick = 1000;
  uint32_t tick_of_jump_request = 0;

  /** Contains the direction from the colliding wall. */
  bool wall_jump_to_right = false;

  Acceleration acceleration_direction = Acceleration::None;

  /** Right direction perpendicular to the slope of the floor. Required for running on slopes. */
  glm::vec2 right_direction{0, 0};
};
} // namespace GameEngine

#endif
