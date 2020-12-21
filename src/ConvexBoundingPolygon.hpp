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

  /** Contains the smallest and largest values found while projecting arbitrary vertices onto an
   * axis. */
  struct ProjectetVerticesMinMax {
    glm::vec2 axis;
    float min; /**< Smallest projected value. */
    float max; /**< Largest projected value. */
  };

  static constexpr size_t edge_count = []() -> size_t {
    if (VertexCount < 2) {
      return 0;
    }
    if (VertexCount == 2) {
      return 1;
    }
    return VertexCount;
  }();

  /** Precomputed normals and projected vertices for all projected_vertices. */
  std::array<ProjectetVerticesMinMax, edge_count> projected_vertices;
};

/** Deduction guide for vertex count template parameter. */
template <typename... T>
ConvexBoundingPolygon(T &&... vertex) -> ConvexBoundingPolygon<sizeof...(T)>;
} // namespace GameEngine

#endif
