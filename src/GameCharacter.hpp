/** @file
 * Contains an interactive character which can run, jump and fall.
 */

#ifndef GAME_ENGINE_SRC_GAME_CHARACTER_HPP
#define GAME_ENGINE_SRC_GAME_CHARACTER_HPP

#include "ConvexBoundingPolygon.hpp"
#include "PhysicalObject.hpp"
#include <glm/vec2.hpp>
#include <optional>

namespace GameEngine {
/** Interactive character which can run, jump and fall. */
class GameCharacter : public PhysicalObject {
public:
  /** Construct a Polygon from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  GameCharacter(std::initializer_list<glm::vec2> vertices);

  /* PhysicalObject functions. */
  void update() override;
  const glm::vec2 &getVelocity() const override;
  virtual void addVelocityOffset(const glm::vec2 &offset) override;
  const ConvexBoundingPolygon &getBoundingPolygon() const override;
  void handleCollisionWith(PhysicalObject &other, const glm::vec2 &displacement_vector) override;

  void jump();

  enum class HorizontalDirection { None, Left, Right };
  void accelerate(HorizontalDirection direction);

  bool isTouchingGround() const;
  bool isTouchingWall() const;

  /** @return Right direction perpendicular to the slope of the ground on which this object stands.
   * Will contain the X axis if the object is falling. */
  const glm::vec2 getRightDirection() const;

private:
  ConvexBoundingPolygon bounding_polygon;

  /** Speed * direction of this object. */
  glm::vec2 velocity{0.0f, 0.0f};

  uint32_t current_tick = 1000;
  uint32_t tick_of_jump_request = 0;

  bool jumpScheduled() const;

  /** Contains the direction from the colliding wall. */
  bool wall_jump_to_right = false;

  HorizontalDirection acceleration_direction = HorizontalDirection::None;

  /** Contains the normal of the ground if the character is standing on it. */
  std::optional<glm::vec2> ground_normal{};

  bool is_touching_wall = false;
  bool is_touching_ceiling = false;
};
} // namespace GameEngine

#endif
