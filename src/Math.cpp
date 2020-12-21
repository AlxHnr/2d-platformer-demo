/** @file
 * Implements math helper functions.
 */

#include "Math.hpp"
#include <glm/geometric.hpp>

namespace GameEngine::Math {
glm::vec2 computeNormalOfEdge(const glm::vec2 &edge_start, const glm::vec2 &edge_end) {
  return glm::normalize(glm::vec2{edge_start.y - edge_end.y, edge_end.x - edge_start.x});
}

std::pair<float, float> projectVerticesOntoAxisMinMax(nonstd::span<const glm::vec2> vertices,
                                                      const glm::vec2 &axis) {
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
} // namespace GameEngine::Math
