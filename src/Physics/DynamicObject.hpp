/** @file
 * Contains a physical object subject to gravity and horizontal acceleration.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_DYNAMIC_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICS_DYNAMIC_OBJECT_HPP

#include "Physics/Object.hpp"
#include <glm/vec2.hpp>
#include <optional>

namespace GameEngine::Physics {
/** Interactive object subject to gravity and horizontal acceleration. */
class DynamicObject : public Physics::Object {
public:
  /** Construct a dynamic object with the boundaries of the given polygon.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  DynamicObject(std::initializer_list<glm::vec2> vertices);

  void update() override;
  const glm::vec2 &getVelocity() const override;
  virtual void addVelocityOffset(const glm::vec2 &offset) override;
  const ConvexBoundingPolygon &getBoundingPolygon() const override;
  void handleCollisionWith(Physics::Object &other, const glm::vec2 &displacement_vector) override;

  void jump();

  enum class HorizontalDirection { None, Left, Right };
  void accelerate(HorizontalDirection direction);

  bool isTouchingGround() const;
  bool isTouchingWall() const;

  /** @return Right direction orthogonal to the slope of the ground on which this object stands.
   * Will contain the X axis if the object is falling. */
  const glm::vec2 getRightDirection() const;

private:
  ConvexBoundingPolygon bounding_polygon;

  /** Speed * direction of this object. */
  glm::vec2 velocity{0.0f, 0.0f};

  /** Counts each update() call. Set to 1000 ticks in the future to allow tick_of_jump_request to be
   * (re)set to 0. */
  uint32_t current_tick = 1000;

  /** Tick at which jump() was called. Allows pressing the jump button slightly before the ground or
   * wall is touched. */
  uint32_t tick_of_jump_request = 0;

  bool jumpScheduled() const;

  HorizontalDirection acceleration_direction = HorizontalDirection::None;

  /** Contains the normal of the ground if the object is standing on it. */
  std::optional<glm::vec2> ground_normal{};

  /** Contains the direction to the wall if the object is touching it. Required for proper wall
   * jumps. */
  HorizontalDirection current_sticky_wall_direction{};

  /** True if the object is jumping up into an object. */
  bool is_touching_ceiling = false;
};
} // namespace GameEngine::Physics

#endif
