/** @file
 * Contains a class for dealing with convex polygon collisions.
 */

#ifndef GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP
#define GAME_ENGINE_SRC_CONVEX_BOUNDING_POLYGON_HPP

#include "PolygonEdge.hpp"
#include <array>
#include <glm/vec2.hpp>

namespace GameEngine {
template <size_t VertexCount> class ConvexBoundingPolygon {
public:
  template <typename... T> ConvexBoundingPolygon(const T &... vertex) : vertices{vertex...} {
    static_assert(VertexCount == sizeof...(T),
                  "Vertex count mismatch in ConvexBoundingPolygon constructor");
    if (VertexCount < 2) {
      return;
    }
    for (size_t index = 1; index < vertices.size(); ++index) {
      edges[index - 1] = PolygonEdge{vertices[index - 1], vertices[index]};
    }
    if (VertexCount > 2) {
      edges.back() = PolygonEdge{vertices.back(), vertices.front()};
    }
  }

  static constexpr size_t edge_count = []() -> size_t {
    if (VertexCount < 2) {
      return 0;
    }
    if (VertexCount == 2) {
      return 1;
    }
    return VertexCount;
  }();

  const std::array<glm::vec2, VertexCount> &getVertices() const { return vertices; }
  const std::array<PolygonEdge, edge_count> &getEdges() const { return edges; }

private:
  std::array<glm::vec2, VertexCount> vertices;

  /** Edges and Precomputed normals for all vertices. */
  std::array<PolygonEdge, edge_count> edges;
};

/** Deduction guide for vertex count template parameter. */
template <typename... T>
ConvexBoundingPolygon(T &&... vertex) -> ConvexBoundingPolygon<sizeof...(T)>;
} // namespace GameEngine

#endif
