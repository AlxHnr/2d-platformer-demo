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
std::pair<glm::vec2, glm::vec2> getEdge(nonstd::span<const glm::vec2> polygon, const size_t index) {
  SDL_assert(polygon.size() >= 2);

  if (index == polygon.size() - 1) {
    return {polygon.back(), polygon.front()};
  }
  return {polygon[index], polygon[index + 1]};
}

glm::vec2 getEdgeNormalAtIndex(nonstd::span<const glm::vec2> polygon, const size_t index) {
  const auto [start, end] = getEdge(polygon, index);
  return computeNormalOfEdge(start, end);
}

float projectOverlap(const glm::vec2 &axis, nonstd::span<const glm::vec2> polygon_a,
                     nonstd::span<const glm::vec2> polygon_b) {
  const auto polygon_a_projected = projectVerticesOntoAxisMinMax(polygon_a, axis);
  const auto polygon_b_projected = projectVerticesOntoAxisMinMax(polygon_b, axis);
  return glm::min(polygon_b_projected.max - polygon_a_projected.min,
                  polygon_a_projected.max - polygon_b_projected.min);
}

struct Overlap {
  float magnitude;
  glm::vec2 direction;
};

std::optional<Overlap> findSmallestOverlap(nonstd::span<const glm::vec2> polygon_a,
                                           nonstd::span<const glm::vec2> polygon_b) {
  if (polygon_a.empty()) {
    return std::nullopt;
  }

  /** Use X axis as direction for polygons with only one vertex. */
  auto direction_of_smallest_overlap =
      polygon_a.size() == 1 ? glm::vec2{1, 0} : getEdgeNormalAtIndex(polygon_a, 0);
  auto smallest_overlap = projectOverlap(direction_of_smallest_overlap, polygon_a, polygon_b);
  if (smallest_overlap < 0) {
    return std::nullopt;
  }

  const auto polygon_a_edges = countEdges(polygon_a);
  for (size_t index = 1; index < polygon_a_edges; ++index) {
    const auto axis = getEdgeNormalAtIndex(polygon_a, index);
    const auto overlap = projectOverlap(axis, polygon_a, polygon_b);
    if (overlap < 0) {
      return std::nullopt;
    }
    if (overlap < smallest_overlap) {
      smallest_overlap = overlap;
      direction_of_smallest_overlap = axis;
    }
  }

  return Overlap{smallest_overlap, direction_of_smallest_overlap};
}

glm::vec2 computeCenterOfPolygon(nonstd::span<const glm::vec2> polygon) {
  return std::accumulate(polygon.begin(), polygon.end(), glm::vec2{}) /
         static_cast<float>(polygon.size());
}
} // namespace

namespace GameEngine::Geometry {
void forEachEdge(nonstd::span<const glm::vec2> polygon,
                 const std::function<void(const size_t edge_index, const glm::vec2 &edge_start,
                                          const glm::vec2 &edge_end)> &function) {
  const auto edge_count = countEdges(polygon);
  for (size_t index = 0; index < edge_count; ++index) {
    const auto [start, end] = getEdge(polygon, index);
    function(index, start, end);
  }
}

glm::vec2 computeNormalOfEdge(const glm::vec2 &edge_start, const glm::vec2 &edge_end) {
  return glm::normalize(glm::vec2{edge_start.y - edge_end.y, edge_end.x - edge_start.x});
}

ProjectedVertices projectVerticesOntoAxisMinMax(nonstd::span<const glm::vec2> vertices,
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

std::optional<glm::vec2> checkPolygonCollision(nonstd::span<const glm::vec2> polygon_a,
                                               nonstd::span<const glm::vec2> polygon_b) {
  if (polygon_a.empty() || polygon_b.empty()) {
    return std::nullopt;
  }

  const auto polygon_b_projected_onto_a = findSmallestOverlap(polygon_a, polygon_b);
  if (!polygon_b_projected_onto_a) {
    return std::nullopt;
  }

  const auto polygon_a_projected_onto_b = findSmallestOverlap(polygon_b, polygon_a);
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
