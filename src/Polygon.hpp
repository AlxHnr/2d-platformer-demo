/** @file
 * Contains a polygon class.
 */

#ifndef GAME_ENGINE_SRC_POLYGON_HPP
#define GAME_ENGINE_SRC_POLYGON_HPP

#include "Geometry.hpp"
#include <glm/vec2.hpp>
#include <vector>

namespace GameEngine {
/** Represents a polygon in the game world which can be moved and rotated. */
class Polygon {
public:
  /** Construct a polygon from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  Polygon(std::initializer_list<glm::vec2> vertices);

  /** @return Borders of the object in the game world used for collision detection. */
  Geometry::ConvexPolygonView getBoundingPolygon() const;

  /** @return Center of the object in the game world. */
  const glm::vec2 &getPosition() const;

  /** @param position New position (center) of the object in the game world. */
  void setPosition(const glm::vec2 &position);

  /** @return Orientation of this polygon in radians. */
  float getOrientation() const;

  /** Set the orientation/angle at which this polygon is rotated around its center.
   *
   * @param orientation New orientation in radians.
   */
  void setOrientation(float orientation);

private:
  /** Center of this object. */
  glm::vec2 position;

  /** Angle at which the polygon was rotated around its center. */
  float orientation{0.0f};

  /** Used for collision detection. */
  std::vector<glm::vec2> bounding_polygon;

  /** Used for transformations. */
  std::vector<glm::vec2> bounding_polygon_relative_to_center;

  void recomputeBoundingPolygon();
};
} // namespace GameEngine

#endif
