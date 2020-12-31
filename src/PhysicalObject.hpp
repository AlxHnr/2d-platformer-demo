/** @file
 * Declares an interactive polygon.
 */

#ifndef GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP

#include "Geometry.hpp"
#include <glm/vec2.hpp>
#include <vector>

namespace GameEngine {
/** Basic convex polygon to which physics and collisions can be applied. */
class PhysicalObject {
public:
  /** Construct a polygon from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  PhysicalObject(std::initializer_list<glm::vec2> vertices);

  void enablePhysics();

  /** Rotate this polygon around its center.
   *
   * @param angle Rotation in radians.
   */
  void rotate(float angle);

  void jump();

  enum class Acceleration { None, Left, Right };
  void accelerate(Acceleration direction);

  /** @return Center of the object in the game world. */
  const glm::vec2 &getPosition() const;

  /** @param new_position New position (center) of the object in the game world. */
  void setPosition(const glm::vec2 &new_position);

  /** @return Velocity of the object. */
  const glm::vec2 &getVelocity() const;

  /** @param new_velocity New velocity of the object. */
  void setVelocity(const glm::vec2 &new_velocity);

  /** @return Borders of the object in the game world used for collision detection. */
  Geometry::ConvexPolygonView getBoundingPolygon() const;

  /** Will be called every tick to update the state of the object. */
  void update();

  /** Gets called when a collision occurred.
   *
   * @param other Object which collided with this object.
   * @param displacement_vector Offset required to move this object out of the other object.
   */
  void handleCollision(PhysicalObject &other, const glm::vec2 &displacement_vector);

private:
  bool physics_enabled = false;

  /** Center of this object. */
  glm::vec2 position;

  /** Angle at which the polygon was rotated around its center. */
  float orientation{0.0f};

  /** Speed * direction of this object. */
  glm::vec2 velocity{0.0f, 0.0f};

  /** Vertices of the polygon in the game world used for collision detection. */
  std::vector<glm::vec2> vertices;

  /** Vertices relative to the center of the polygon. Used for transformations. */
  std::vector<glm::vec2> vertices_relative_to_center;

  void recomputeBoundingBox();

  uint32_t current_tick = 1000;
  uint32_t tick_of_jump_request = 0;
  uint32_t tick_of_last_floor_collision = 0;
  uint32_t tick_of_last_wall_collision = 0;

  /** Contains the direction from the colliding wall. */
  bool wall_jump_to_right = false;

  Acceleration acceleration_direction = Acceleration::None;

  /** Right direction perpendicular to the slope of the floor. Required for running on slopes. */
  glm::vec2 right_direction{0, 0};

  bool jumpScheduled() const;
  bool sticksToFloor() const;
  bool sticksToWall() const;
};
} // namespace GameEngine

#endif
