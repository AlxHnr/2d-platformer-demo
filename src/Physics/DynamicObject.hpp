/** @file
 * Contains a physical object subject to gravity.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_DYNAMIC_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICS_DYNAMIC_OBJECT_HPP

#include "Physics/Object.hpp"
#include <glm/vec2.hpp>
#include <optional>

namespace GameEngine::Physics {
/** Interactive object subject to gravity. */
class DynamicObject : public Object {
public:
  /** Construct a dynamic object with the boundaries of the given polygon.
   *
   * @param vertices Zero or more points representing a convex polygon.
   */
  DynamicObject(std::initializer_list<glm::vec2> vertices);

  bool isTouchingGround() const;

  /** @return Direction to the colliding wall, if the object is colliding with a wall. */
  std::optional<glm::vec2> isTouchingWall() const;

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
  void render(SDL_Renderer *renderer, const Camera &camera,
              float integrator_tick_blend_value) const override;

  glm::vec2 getVelocity() const override;
  void setVelocity(glm::vec2 velocity);

  /** @return Positive value, continuously applied to the object orthogonal to the current slope. */
  float getGravity() const;

  /** @param gravity Positive value, continuously applied to the object orthogonal to the current
   * slope. If negative, will be set to zero. */
  void setGravity(float gravity);

  /** @return Value between 0 and 1, applied to the object while touching the ground. */
  float getGroundStickiness() const;

  /** @param ground_stickiness Value between 0 and 1, applied to the object while touching the
   * ground. Will be clamped to the expected range. */
  void setGroundStickiness(float ground_stickiness);

  /** @return Value between 0 and 1, applied to the object while sliding on a wall. */
  float getWallStickiness() const;

  /** @param wall_stickiness Value between 0 and 1, applied to the object while sliding on a wall.
   * Will be clamped to the expected range. */
  void setWallStickiness(float wall_stickiness);

  /** @return Value between 0 and 1, continuously applied to the object. */
  float getAirFriction() const;

  /** @param air_friction Value between 0 and 1, continuously applied to the object. Will be clamped
   * to the expected range. */
  void setAirFriction(float air_friction);

private:
  ConvexBoundingPolygon bounding_polygon;

  /** Current direction * speed of this object. */
  glm::vec2 velocity = {0, 0};

  /** Positive value, continuously applied to the object orthogonal to the current slope. */
  float gravity = 0.0125;

  /** Value between 0 and 1, applied to the object while touching the ground. */
  float ground_stickiness = 0.05;

  /** Value between 0 and 1, applied to the object while sliding on a wall. */
  float wall_stickiness = 0;

  /** Value between 0 and 1, continuously applied to the object. */
  float air_friction = 0.01;

  /** Contains the normal of the ground if the object is standing on it. */
  std::optional<glm::vec2> ground_normal = std::nullopt;

  /** Contains the direction to the wall if the object is touching it. */
  std::optional<glm::vec2> direction_to_colliding_wall = std::nullopt;

  /** True if the object is hitting another object from below. */
  bool is_touching_ceiling = false;

  /** Used for tick-independent rendering by interpolating with the current state. */
  struct {
    bool touching_ground;
    bool touching_wall;
    glm::vec2 velocity;
    glm::vec2 right_direction;
    glm::vec2 bounding_polygon_position;
  } state_at_previous_tick;

  void storeCurrentStateAsPrevious();
};
} // namespace GameEngine::Physics

#endif
