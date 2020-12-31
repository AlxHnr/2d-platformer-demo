/** @file
 * Contains a polygon class representing collision boundaries.
 */

#ifndef GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP
#define GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP

#include <functional>
#include <glm/vec2.hpp>
#include <vector>

namespace GameEngine {
/** Represents a convex bounding polygon in the game world for collision detection. */
class ConvexBoundingPolygon {
public:
  /** Construct a polygon from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon in the game world. If no
   * points are provided, it will behave like a non-existing dummy.
   */
  ConvexBoundingPolygon(std::initializer_list<glm::vec2> vertices);

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

  /** Apply the given function to all edges of this polygon.
   *
   * @param function Will be called on each edge. Takes the start and end position of the current
   * edge.
   */
  void forEachEdge(const std::function<void(const glm::vec2 &edge_start, const glm::vec2 &edge_end)>
                       &function) const;

  /** Check if this polygon collides with another. Uses the separating axis theorem.
   *
   * @param other Polygon to check against.
   *
   * @return Displacement vector (MTV) for moving this polygon out of the other polygon. This
   * function will return nothing if no collision occurred.
   *
   * Usage examples:
   *
   * @code
   * Polygon my_triangle{
   *     {575, 400},
   *     {792, 515},
   *     {870, 670},
   * };
   *
   * Polygon my_line{
   *     {575, 400},
   *     {870, 670},
   * };
   *
   * if(my_triangle.collidesWith(my_line) {
   *   ...
   * }
   * @endcode
   */
  std::optional<glm::vec2> collidesWith(const ConvexBoundingPolygon &other) const;

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
