/** @file
 * Implements math helper functions.
 */

#include "Geometry.hpp"
#include <SDL_assert.h>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/vector_query.hpp>

namespace {
/** Performs a partial collision check using the separating axis theorem. It only matches the minmax
 * values of the first polygon against the second polygon. For a complete collision check it is also
 * required to match the minmax values of the second polygon against the first polygon.
 *
 * @param projected_minmax_values_of_a Precomputed axes and projected minmax values of the first
 * polygon.
 * @param polygon_b All vertices of the second polygon.
 *
 * @return True if a collision was detected.
 */
bool checkPartialSATCollision(
    nonstd::span<const GameEngine::Geometry::ProjectetVerticesMinMax> projected_minmax_values_of_a,
    nonstd::span<const glm::vec2> polygon_b) {
  const size_t all_axes_overlap =
      std::all_of(projected_minmax_values_of_a.begin(), projected_minmax_values_of_a.end(),
                  [&](const auto &projected_vertices) {
                    const auto [min, max] = GameEngine::Geometry::projectVerticesOntoAxisMinMax(
                        polygon_b, projected_vertices.axis);
                    return projected_vertices.min < max && projected_vertices.max > min;
                  });
  return !projected_minmax_values_of_a.empty() && all_axes_overlap;
}
} // namespace

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

bool checkPolygonCollision(
    nonstd::span<const glm::vec2> polygon_a,
    nonstd::span<const ProjectetVerticesMinMax> projected_minmax_values_of_a,
    nonstd::span<const glm::vec2> polygon_b,
    nonstd::span<const ProjectetVerticesMinMax> projected_minmax_values_of_b) {
  return checkPartialSATCollision(projected_minmax_values_of_a, polygon_b) &&
         checkPartialSATCollision(projected_minmax_values_of_b, polygon_a);
}
} // namespace GameEngine::Geometry
