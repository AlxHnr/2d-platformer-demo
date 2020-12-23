/** @file
 * Implements math helper functions.
 */

#include "Geometry.hpp"
#include <SDL_assert.h>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/vector_query.hpp>

namespace GameEngine::Geometry {
void forEachEdge(nonstd::span<const glm::vec2> vertices,
                 const std::function<void(const size_t edge_index, const glm::vec2 &edge_start,
                                          const glm::vec2 &edge_end)> &function) {
  if (vertices.size() < 2) {
    return;
  }
  for (size_t index = 1; index < vertices.size(); ++index) {
    function(index - 1, vertices[index - 1], vertices[index]);
  }
  if (vertices.size() > 2) {
    function(vertices.size() - 1, vertices.back(), vertices.front());
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

std::optional<glm::vec2>
checkPolygonCollision(nonstd::span<const glm::vec2> polygon_a,
                      nonstd::span<const ProjectedVertices> polygon_a_projected,
                      nonstd::span<const glm::vec2> polygon_b,
                      nonstd::span<const ProjectedVertices> polygon_b_projected) {
  if (polygon_a_projected.empty() || polygon_b_projected.empty()) {
    return std::nullopt;
  }

  const auto compute_overlap = [](const ProjectedVertices &polygon_projected,
                                  nonstd::span<const glm::vec2> other_polygon) {
    const auto other_polygon_projected =
        projectVerticesOntoAxisMinMax(other_polygon, polygon_projected.axis);
    return glm::min(other_polygon_projected.max - polygon_projected.min,
                    polygon_projected.max - other_polygon_projected.min);
  };

  auto smallest_overlap = compute_overlap(polygon_a_projected.front(), polygon_b);
  auto direction_of_smallest_overlap = polygon_a_projected.front().axis;
  if (smallest_overlap < 0) {
    return std::nullopt;
  }
  const auto update_smallest_overlap = [&](const ProjectedVertices &polygon_projected,
                                           nonstd::span<const glm::vec2> other_polygon,
                                           const float invert_axis_factor) {
    const auto overlap = compute_overlap(polygon_projected, other_polygon);
    if (overlap < 0) {
      return false;
    }
    if (overlap < smallest_overlap) {
      smallest_overlap = overlap;
      direction_of_smallest_overlap = polygon_projected.axis * invert_axis_factor;
    }
    return true;
  };

  for (size_t index = 1; index < polygon_a_projected.size(); ++index) {
    if (!update_smallest_overlap(polygon_a_projected[index], polygon_b, 1)) {
      return std::nullopt;
    }
  }
  for (const auto &projected : polygon_b_projected) {
    if (!update_smallest_overlap(projected, polygon_a, -1)) {
      return std::nullopt;
    }
  }
  return smallest_overlap * -direction_of_smallest_overlap;
}
} // namespace GameEngine::Geometry
