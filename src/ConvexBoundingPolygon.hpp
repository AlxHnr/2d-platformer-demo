/** @file
 * Contains a class for dealing with convex polygon collisions.
 */

#ifndef GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP
#define GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP

#include "Math.hpp"
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
   * Usage example:
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
    Math::forEachEdge(vertices,
                      [&](const size_t index, const glm::vec2 &start, const glm::vec2 &end) {
                        const auto axis = Math::computeNormalOfEdge(start, end);
                        const auto [min, max] = Math::projectVerticesOntoAxisMinMax(vertices, axis);
                        projected_vertices[index] = {axis, min, max};
                      });
  }

  const std::array<glm::vec2, VertexCount> &getVertices() const { return vertices; }

private:
  std::array<glm::vec2, VertexCount> vertices;

  static constexpr size_t edge_count = []() -> size_t {
    if (VertexCount < 2) {
      return 0;
    }
    if (VertexCount == 2) {
      return 1;
    }
    return VertexCount;
  }();

  /** Precomputed normals and projected vertices for all normal axes of the polygon. */
  std::array<Math::ProjectetVerticesMinMax, edge_count> projected_vertices;
};

/** Deduction guide for vertex count template parameter. Allows construction without specifying a
 * vertex count. */
template <typename... T>
ConvexBoundingPolygon(T &&... vertex) -> ConvexBoundingPolygon<sizeof...(T)>;
} // namespace GameEngine

#endif
