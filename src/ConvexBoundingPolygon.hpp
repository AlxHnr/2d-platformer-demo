/** @file
 * Contains a class for dealing with convex polygon collisions.
 */

#ifndef GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP
#define GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP

#include "Geometry.hpp"
#include <array>
#include <glm/vec2.hpp>

namespace GameEngine {
template <size_t VertexCount> class ConvexBoundingPolygon {
public:
  /** Constructs a convex bounding polygon for collision detection.
   *
   * @tparam T Must be glm::vec2.
   * @param Vertex Zero or more edges/points. E.g. 4 points represent a quad. These points must
   * represent a convex polygon for accurate collision detection.
   *
   * Usage examples:
   *
   * @code
   * ConvexBoundingPolygon my_triangle{
   *     glm::vec2{575, 400},
   *     glm::vec2{792, 515},
   *     glm::vec2{870, 670},
   * };
   *
   * ConvexBoundingPolygon my_line{
   *     glm::vec2{575, 400},
   *     glm::vec2{870, 670},
   * };
   *
   * ConvexBoundingPolygon my_point{
   *     glm::vec2{575, 400},
   * };
   * @endcode
   */
  template <typename... T> ConvexBoundingPolygon(const T &... vertex) : vertices{vertex...} {
    static_assert(VertexCount == sizeof...(T),
                  "Vertex count mismatch in ConvexBoundingPolygon constructor");
    Geometry::forEachEdge(
        vertices, [&](const size_t index, const glm::vec2 &start, const glm::vec2 &end) {
          const auto axis = Geometry::computeNormalOfEdge(start, end);
          projected_vertices[index] = Geometry::projectVerticesOntoAxisMinMax(vertices, axis);
        });
  }

  /** Overload for points. Uses the Y axis for projection. */
  ConvexBoundingPolygon(const glm::vec2 &vertex) : vertices{vertex} {
    projected_vertices[0] = Geometry::projectVerticesOntoAxisMinMax(vertices, {0, 1});
  }

  /** Check if this polygon collides with another polygon.
   *
   * @tparam OtherVertexCount Amount of vertices in the other polygon.
   * @param other Polygon to check against.
   *
   * @return Displacement vector (MTV) for moving this polygon out of the other polygon.
   *
   * Usage example:
   *
   * @code
   * ConvexBoundingPolygon my_triangle{...};
   * ConvexBoundingPolygon my_quad{...};
   *
   * if(const auto displacement = my_triangle.collidesWith(my_quad)) {
   *   // Move object represented by my_triangle out of my_quad by adding *displacement to
   *   // my_triangles position.
   * }
   * @code
   */
  template <size_t OtherVertexCount>
  std::optional<glm::vec2>
  collidesWith(const ConvexBoundingPolygon<OtherVertexCount> &other) const {
    return Geometry::checkPolygonCollision(vertices, projected_vertices, other.vertices,
                                           other.projected_vertices);
  }

  const std::array<glm::vec2, VertexCount> &getVertices() const { return vertices; }

private:
  std::array<glm::vec2, VertexCount> vertices;

  static constexpr size_t edge_count = []() -> size_t {
    if (VertexCount == 0) {
      return 0;
    }
    if (VertexCount <= 2) {
      return 1;
    }
    return VertexCount;
  }();

  /** Precomputed normals and projected vertices for all axes of the polygon. */
  std::array<Geometry::ProjectedVertices, edge_count> projected_vertices;

  /** Allow collision check with polygons of other size. */
  template <size_t OtherVertexCount> friend class ConvexBoundingPolygon;
};

/** Deduction guide for vertex count template parameter. Allows construction without specifying a
 * vertex count. */
template <typename... T>
ConvexBoundingPolygon(const T &... vertex) -> ConvexBoundingPolygon<sizeof...(T)>;
} // namespace GameEngine

#endif
