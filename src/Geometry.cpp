/** @file
 * Implements math helper functions.
 */

#include "Geometry.hpp"
#include "glm/gtc/constants.hpp"
#include <SDL_assert.h>
#include <glm/gtx/vector_query.hpp>
#include <numeric>

namespace {
using namespace GameEngine::Geometry;

/** Count the edges of the given polygon.
 *
 * @param polygon Contains zero or more vertices.
 *
 * @return Amount of edges in the given polygon. E.g. a triangle with 3 points has 3 edges. A line
 * with 2 points has one edge.
 */
size_t countEdges(nonstd::span<const glm::vec2> polygon) {
  if (polygon.size() < 2) {
    return 0;
  }
  if (polygon.size() == 2) {
    return 1;
  }
  return polygon.size();
}
/** Get the edge from the given polygon specified by the edges index.
 *
 * @param polygon Contains at least two vertices representing an edge.
 * @param edge_index Index of the edge starting at 0. Must be inside the bounds of the given
 * polygon. See countEdges().
 *
 * @return [start, end] positions of the polygons nth edge.
 */
std::pair<glm::vec2, glm::vec2> getEdge(nonstd::span<const glm::vec2> polygon,
                                        const size_t edge_index) {
  if (edge_index == polygon.size() - 1) {
    return {polygon.back(), polygon.front()};
  }
  return {polygon[edge_index], polygon[edge_index + 1]};
}

/** @return Normal vector orthogonal to the polygons nth edge. */
glm::vec2 getEdgeNormal(ConvexPolygonView polygon, const size_t edge_index) {
  const auto [start, end] = getEdge(polygon, edge_index);
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
  if (smallest_overlap < glm::epsilon<float>()) {
    return std::nullopt;
  }

  const auto a_edges = countEdges(a);
  for (size_t index = 1; index < a_edges; ++index) {
    const auto axis = getEdgeNormal(a, index);
    const auto overlap = getProjectionOverlap(a, b, axis);
    if (overlap < glm::epsilon<float>()) {
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

namespace GameEngine::Geometry {
void forEachEdge(
    nonstd::span<const glm::vec2> polygon,
    const std::function<void(const glm::vec2 &edge_start, const glm::vec2 &edge_end)> &function) {
  const auto edge_count = countEdges(polygon);
  for (size_t index = 0; index < edge_count; ++index) {
    const auto [start, end] = getEdge(polygon, index);
    function(start, end);
  }
}

glm::vec2 computeCenter(ConvexPolygonView polygon) {
  SDL_assert(!polygon.empty());
  return std::accumulate(polygon.cbegin(), polygon.cend(), glm::vec2{}) /
         static_cast<float>(polygon.size());
}

std::optional<glm::vec2> checkCollision(ConvexPolygonView a, ConvexPolygonView b) {
  if (a.empty() || b.empty()) {
    return std::nullopt;
  }

  const auto displacement_a_from_b = findSmallestDisplacementVector(a, b);
  if (!displacement_a_from_b) {
    return std::nullopt;
  }

  const auto displacement_b_from_a = findSmallestDisplacementVector(b, a);
  if (!displacement_b_from_a) {
    return std::nullopt;
  }

  const auto displacement_vector = [&] {
    if (displacement_a_from_b->magnitude < displacement_b_from_a->magnitude) {
      SDL_assert(displacement_a_from_b->magnitude > glm::epsilon<float>());
      return displacement_a_from_b->direction * displacement_a_from_b->magnitude;
    }
    SDL_assert(displacement_b_from_a->magnitude > glm::epsilon<float>());
    return -displacement_b_from_a->direction * displacement_b_from_a->magnitude;
  }();

  const auto direction_from_a_to_b = computeCenter(b) - computeCenter(a);
  if (glm::dot(direction_from_a_to_b, displacement_vector) < 0) {
    return displacement_vector;
  }
  return -displacement_vector;
}
} // namespace GameEngine::Geometry
