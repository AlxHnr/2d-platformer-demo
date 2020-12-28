/** @file
 * Declares an interactive polygon.
 */

#ifndef GAME_ENGINE_SRC_POLYGON_OBJECT_HPP
#define GAME_ENGINE_SRC_POLYGON_OBJECT_HPP

#include "PhysicsObject.hpp"
#include <vector>

namespace GameEngine {
/** Basic convex polygon to which physics and collisions can be applied. */
class PolygonObject : public PhysicsObject {
public:
  /** Construct a polygon from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  PolygonObject(std::initializer_list<glm::vec2> vertices);

  /** Rotate this polygon around its center.
   *
   * @param angle Rotation in radians.
   */
  void rotate(float angle);

  /* PhysicsObject interface functions. */
  const glm::vec2 &getPosition() const override;
  void setPosition(const glm::vec2 &new_position) override;
  const glm::vec2 &getVelocity() const override;
  void setVelocity(const glm::vec2 &new_velocity) override;
  Geometry::ConvexPolygonView getBoundingPolygon() const override;

private:
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
};
} // namespace GameEngine

#endif
