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

std::pair<float, float> projectVerticesOntoAxisMinMax(nonstd::span<const glm::vec2> vertices,
                                                      const glm::vec2 &axis) {
  SDL_assert(glm::isNormalized(axis, glm::epsilon<float>()));

  if (vertices.empty()) {
    return {};
  }

  const auto first_dot_product = glm::dot(vertices.front(), axis);
  float min = first_dot_product;
  float max = first_dot_product;

  for (size_t index = 1; index < vertices.size(); ++index) {
    const auto dot_product = glm::dot(vertices[index], axis);
    min = std::min(min, dot_product);
    max = std::max(max, dot_product);
  }
  return {min, max};
}

std::optional<glm::vec2>
checkPolygonCollision(nonstd::span<const glm::vec2> polygon_a,
                      nonstd::span<const ProjectedVerticesMinMax> polygon_a_minmax,
                      nonstd::span<const glm::vec2> polygon_b,
                      nonstd::span<const ProjectedVerticesMinMax> polygon_b_minmax) {
  if (polygon_a_minmax.empty() || polygon_b_minmax.empty()) {
    return std::nullopt;
  }

  const auto compute_overlap = [](const ProjectedVerticesMinMax &minmax,
                                  nonstd::span<const glm::vec2> other_polygon) {
    const auto [min, max] = projectVerticesOntoAxisMinMax(other_polygon, minmax.axis);
    return glm::min(max - minmax.min, minmax.max - min);
  };

  auto smallest_overlap = compute_overlap(polygon_a_minmax.front(), polygon_b);
  auto direction_of_smallest_overlap = polygon_a_minmax.front().axis;
  if (smallest_overlap < 0) {
    return std::nullopt;
  }
  const auto update_smallest_overlap = [&](const ProjectedVerticesMinMax &minmax,
                                           nonstd::span<const glm::vec2> other_polygon,
                                           const float invert_axis_factor) {
    const auto overlap = compute_overlap(minmax, other_polygon);
    if (overlap < 0) {
      return false;
    }
    if (overlap < smallest_overlap) {
      smallest_overlap = overlap;
      direction_of_smallest_overlap = minmax.axis * invert_axis_factor;
    }
    return true;
  };

  for (size_t index = 1; index < polygon_a_minmax.size(); ++index) {
    if (!update_smallest_overlap(polygon_a_minmax[index], polygon_b, 1)) {
      return std::nullopt;
    }
  }
  for (const auto &minmax : polygon_b_minmax) {
    if (!update_smallest_overlap(minmax, polygon_a, -1)) {
      return std::nullopt;
    }
  }
  return smallest_overlap * -direction_of_smallest_overlap;
}
} // namespace GameEngine::Geometry
