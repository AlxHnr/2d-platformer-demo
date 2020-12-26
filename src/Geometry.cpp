/** @file
 * Implements math helper functions.
 */

#include "Geometry.hpp"
#include <SDL_assert.h>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/vector_query.hpp>
#include <numeric>

namespace {
using namespace GameEngine::Geometry;

size_t countEdges(nonstd::span<const glm::vec2> polygon) {
  if (polygon.size() < 2) {
    return 0;
  }
  if (polygon.size() == 2) {
    return 1;
  }
  return polygon.size();
}

/** @return [start, end] positions of a polygons nth edge. */
std::pair<glm::vec2, glm::vec2> getEdge(nonstd::span<const glm::vec2> polygon,
                                        const size_t edge_index) {
  SDL_assert(polygon.size() >= 2);

  if (edge_index == polygon.size() - 1) {
    return {polygon.back(), polygon.front()};
  }
  return {polygon[edge_index], polygon[edge_index + 1]};
}

/** @return Normal vector orthogonal to the polygons nth edge. */
glm::vec2 getEdgeNormal(nonstd::span<const glm::vec2> polygon, const size_t edge_index) {
  const auto [start, end] = getEdge(polygon, edge_index);
  return glm::normalize(glm::vec2{start.y - end.y, end.x - start.x});
}

/** Contains the smallest and largest values found while projecting vertices onto an axis. */
struct ProjectedVertices {
  glm::vec2 axis; /**< Normalized axis onto which vertices got projected. */
  float min;      /**< Smallest projected value. */
  float max;      /**< Largest projected value. */
};

ProjectedVertices projectVerticesOntoAxis(nonstd::span<const glm::vec2> vertices,
                                          const glm::vec2 &axis) {
  SDL_assert(!vertices.empty());
  SDL_assert(glm::isNormalized(axis, glm::epsilon<float>()));

  const auto first_dot_product = glm::dot(vertices.front(), axis);
  float min = first_dot_product;
  float max = first_dot_product;

  for (size_t index = 1; index < vertices.size(); ++index) {
    const auto dot_product = glm::dot(vertices[index], axis);
    min = std::min(min, dot_product);
    max = std::max(max, dot_product);
  }
  return {axis, min, max};
}

/** @return Overlap found while projecting the given polygons onto the specified axis. Will be < 0
 * if no overlap exists. */
float getProjectionOverlap(nonstd::span<const glm::vec2> polygon_a,
                           nonstd::span<const glm::vec2> polygon_b, const glm::vec2 &axis) {
  const auto polygon_a_projected = projectVerticesOntoAxis(polygon_a, axis);
  const auto polygon_b_projected = projectVerticesOntoAxis(polygon_b, axis);
  return glm::min(polygon_b_projected.max - polygon_a_projected.min,
                  polygon_a_projected.max - polygon_b_projected.min);
}

/** Offset for moving one polygon out of another. */
struct DisplacementVector {
  glm::vec2 direction;
  float magnitude;
};

/** @return Smallest displacement vector (MTV) for moving polygon_a out of polygon_b. Will return
 * nothing if no collision occurred. */
std::optional<DisplacementVector>
findSmallestDisplacementVector(nonstd::span<const glm::vec2> polygon_a,
                               nonstd::span<const glm::vec2> polygon_b) {
  if (polygon_a.empty()) {
    return std::nullopt;
  }

  /** Use X axis as direction for polygons with only one vertex. */
  auto direction_of_smallest_overlap =
      polygon_a.size() == 1 ? glm::vec2{1, 0} : getEdgeNormal(polygon_a, 0);
  auto smallest_overlap = getProjectionOverlap(polygon_a, polygon_b, direction_of_smallest_overlap);
  if (smallest_overlap < 0) {
    return std::nullopt;
  }

  const auto polygon_a_edges = countEdges(polygon_a);
  for (size_t index = 1; index < polygon_a_edges; ++index) {
    const auto axis = getEdgeNormal(polygon_a, index);
    const auto overlap = getProjectionOverlap(polygon_a, polygon_b, axis);
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

glm::vec2 computeCenterOfPolygon(nonstd::span<const glm::vec2> polygon) {
  return std::accumulate(polygon.begin(), polygon.end(), glm::vec2{}) /
         static_cast<float>(polygon.size());
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

std::optional<glm::vec2> checkCollision(nonstd::span<const glm::vec2> polygon_a,
                                        nonstd::span<const glm::vec2> polygon_b) {
  if (polygon_a.empty() || polygon_b.empty()) {
    return std::nullopt;
  }

  const auto polygon_b_projected_onto_a = findSmallestDisplacementVector(polygon_a, polygon_b);
  if (!polygon_b_projected_onto_a) {
    return std::nullopt;
  }

  const auto polygon_a_projected_onto_b = findSmallestDisplacementVector(polygon_b, polygon_a);
  if (!polygon_a_projected_onto_b) {
    return std::nullopt;
  }

  const auto displacement_vector = [&] {
    if (polygon_b_projected_onto_a->magnitude < polygon_a_projected_onto_b->magnitude) {
      return polygon_b_projected_onto_a->direction * polygon_b_projected_onto_a->magnitude;
    }
    return -polygon_a_projected_onto_b->direction * polygon_a_projected_onto_b->magnitude;
  }();

  const auto direction_from_a_to_b =
      computeCenterOfPolygon(polygon_b) - computeCenterOfPolygon(polygon_a);
  if (glm::dot(direction_from_a_to_b, displacement_vector) < 0) {
    return displacement_vector;
  }
  return -displacement_vector;
}
} // namespace GameEngine::Geometry
