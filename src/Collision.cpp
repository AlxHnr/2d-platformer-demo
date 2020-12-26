/** @file
 * Implements collision detection between convex polygons using the separating axis theorem.
 */

#include "Collision.hpp"
#include "Geometry.hpp"
#include <glm/gtx/vector_query.hpp>
#include <numeric>

namespace {
using namespace GameEngine;

glm::vec2 computeCenterOfPolygon(ConvexPolygonView polygon) {
  return std::accumulate(polygon.begin(), polygon.end(), glm::vec2{}) /
         static_cast<float>(polygon.size());
}

/** @return Normal vector orthogonal to the polygons nth edge. */
glm::vec2 getEdgeNormal(ConvexPolygonView polygon, const size_t edge_index) {
  const auto [start, end] = Geometry::getEdge(polygon, edge_index);
  return glm::normalize(glm::vec2{start.y - end.y, end.x - start.x});
}

/** Contains the smallest and largest values found while projecting vertices onto an axis. */
struct ProjectedVertices {
  glm::vec2 axis; /**< Normalized axis onto which vertices got projected. */
  float min;      /**< Smallest projected value. */
  float max;      /**< Largest projected value. */
};

ProjectedVertices projectVerticesOntoAxis(ConvexPolygonView polygon, const glm::vec2 &axis) {
  const auto first_dot_product = glm::dot(polygon.front(), axis);
  float min = first_dot_product;
  float max = first_dot_product;

  for (size_t index = 1; index < polygon.size(); ++index) {
    const auto dot_product = glm::dot(polygon[index], axis);
    min = std::min(min, dot_product);
    max = std::max(max, dot_product);
  }
  return {axis, min, max};
}

/** @return Overlap found while projecting the given polygons onto the specified axis. Will be < 0
 * if no overlap exists. */
float getProjectionOverlap(ConvexPolygonView a, ConvexPolygonView b, const glm::vec2 &axis) {
  const auto a_projected = projectVerticesOntoAxis(a, axis);
  const auto b_projected = projectVerticesOntoAxis(b, axis);
  return glm::min(b_projected.max - a_projected.min, a_projected.max - b_projected.min);
}

/** Offset for moving one polygon out of another. */
struct DisplacementVector {
  glm::vec2 direction;
  float magnitude;
};

/** @return Smallest displacement vector (MTV) for moving polygon a out of polygon b. Will return
 * nothing if no collision occurred. */
std::optional<DisplacementVector> findSmallestDisplacementVector(ConvexPolygonView a,
                                                                 ConvexPolygonView b) {
  /** Use X axis as direction for polygons with only one vertex. */
  auto direction_of_smallest_overlap = a.size() == 1 ? glm::vec2{1, 0} : getEdgeNormal(a, 0);
  auto smallest_overlap = getProjectionOverlap(a, b, direction_of_smallest_overlap);
  if (smallest_overlap < 0) {
    return std::nullopt;
  }

  const auto a_edges = Geometry::countEdges(a);
  for (size_t index = 1; index < a_edges; ++index) {
    const auto axis = getEdgeNormal(a, index);
    const auto overlap = getProjectionOverlap(a, b, axis);
    if (overlap < 0) {
      return std::nullopt;
    }
    if (overlap < smallest_overlap) {
      smallest_overlap = overlap;
      direction_of_smallest_overlap = axis;
    }
  }

  return DisplacementVector{direction_of_smallest_overlap, smallest_overlap};
}
} // namespace

namespace GameEngine {
std::optional<glm::vec2> checkCollision(ConvexPolygonView a, ConvexPolygonView b) {
  if (a.empty() || b.empty()) {
    return std::nullopt;
  }

  const auto b_projected_onto_a = findSmallestDisplacementVector(a, b);
  if (!b_projected_onto_a) {
    return std::nullopt;
  }

  const auto a_projected_onto_b = findSmallestDisplacementVector(b, a);
  if (!a_projected_onto_b) {
    return std::nullopt;
  }

  const auto displacement_vector = [&] {
    if (b_projected_onto_a->magnitude < a_projected_onto_b->magnitude) {
      return b_projected_onto_a->direction * b_projected_onto_a->magnitude;
    }
    return -a_projected_onto_b->direction * a_projected_onto_b->magnitude;
  }();

  const auto direction_from_a_to_b = computeCenterOfPolygon(b) - computeCenterOfPolygon(a);
  if (glm::dot(direction_from_a_to_b, displacement_vector) < 0) {
    return displacement_vector;
  }
  return -displacement_vector;
}
} // namespace GameEngine
