/** @file
 * Contains a physical object subject to gravity and horizontal acceleration.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_DYNAMIC_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICS_DYNAMIC_OBJECT_HPP

#include "HorizontalDirection.hpp"
#include "Physics/Object.hpp"
#include <glm/vec2.hpp>
#include <optional>

namespace GameEngine::Physics {
/** Interactive object subject to gravity and horizontal acceleration. */
class DynamicObject : public Physics::Object {
public:
  /** Construct a dynamic object with the boundaries of the given polygon.
   *
   * @param vertices Zero or more points representing a convex polygon.
   */
  DynamicObject(std::initializer_list<glm::vec2> vertices);

  bool isTouchingGround() const;

  /** @return Direction to the colliding wall, if the object is colliding with a wall. */
  std::optional<HorizontalDirection> isTouchingWall() const;

  /** @return Normal of the grounds slope on which this object stands. If the object is in the air
   * it will return the Y axis {0, 1}. */
  glm::vec2 getUpDirection() const;

  /** @return Right direction corresponding to the slope of the ground on which this object stands.
   * If the object is in the air it will return the X axis {1, 0}. */
  glm::vec2 getRightDirection() const;

  void update() override;
  virtual void addVelocityOffset(glm::vec2 offset) override;
  const ConvexBoundingPolygon &getBoundingPolygon() const override;
  void handleCollisionWith(Physics::Object &other, glm::vec2 displacement_vector) override;

  glm::vec2 getVelocity() const override;

  /** @return Positive value, continuously applied to the object orthogonal to the current slope. */
  float getGravity() const;

  /** @param gravity Positive value, continuously applied to the object orthogonal to the current
   * slope. If negative, will be set to zero. */
  void setGravity(float gravity);

  /** @return Value between 0 and 1, applied to the object while touching the ground. */
  float getGroundFriction() const;

  /** @param ground_friction Value between 0 and 1, applied to the object while touching the ground.
   * Will be clamped to the expected range. */
  void setGroundFriction(float ground_friction);

  /** @return Value between 0 and 1, applied to the object while sliding down a wall. */
  float getWallFriction() const;

  /** @param wall_friction Value between 0 and 1, applied to the object while sliding down a wall.
   * Will be clamped to the expected range. */
  void setWallFriction(float wall_friction);

  /** @return Value between 0 and 1, continuously applied to the object. */
  float getAirFriction() const;

  /** @param air_friction Value between 0 and 1, continuously applied to the object. Will be clamped
   * to the expected range. */
  void setAirFriction(float air_friction);

  void jump();

  /** @param direction Optional direction to which the object should accelerate. If no direction was
   * given, the object will slow down and stop. The slope of the ground will be considered. */
  void run(std::optional<HorizontalDirection> direction);

private:
  ConvexBoundingPolygon bounding_polygon;

  /** Current direction * speed of this object. */
  glm::vec2 velocity = {0, 0};

  /** Positive value, continuously applied to the object orthogonal to the current slope. */
  float gravity = 0.0125;

  /** Value between 0 and 1, applied to the object while touching the ground. */
  float ground_friction = 0.05;

  /** Value between 0 and 1, applied to the object while sliding down a wall. */
  float wall_friction = 0.03;

  /** Value between 0 and 1, continuously applied to the object. */
  float air_friction = 0.01;

  /** Contains the normal of the ground if the object is standing on it. */
  std::optional<glm::vec2> ground_normal = std::nullopt;

  /** Contains the direction to the wall if the object is touching it. */
  std::optional<HorizontalDirection> direction_to_colliding_wall = std::nullopt;

  /** True if the object is hitting another object from below. */
  bool is_touching_ceiling = false;

  /** Counts each update() call. Set to 1000 ticks in the future to allow tick_of_jump_request to be
   * (re)set to 0. */
  uint32_t current_tick = 1000;

  /** Tick at which jump() was called. Allows pressing the jump button slightly before the ground or
   * wall is touched. */
  uint32_t tick_of_jump_request = 0;

  bool jumpScheduled() const;

  std::optional<HorizontalDirection> acceleration_direction = std::nullopt;
};
} // namespace GameEngine::Physics

#endif
