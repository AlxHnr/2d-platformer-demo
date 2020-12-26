/** @file
 * Implements math helper functions.
 */

#include "Geometry.hpp"
#include <SDL_assert.h>

namespace GameEngine::Geometry {
size_t countEdges(nonstd::span<const glm::vec2> polygon) {
  if (polygon.size() < 2) {
    return 0;
  }
  if (polygon.size() == 2) {
    return 1;
  }
  return polygon.size();
}

std::pair<glm::vec2, glm::vec2> getEdge(nonstd::span<const glm::vec2> polygon,
                                        const size_t edge_index) {
  SDL_assert(polygon.size() >= 2);

  if (edge_index == polygon.size() - 1) {
    return {polygon.back(), polygon.front()};
  }
  return {polygon[edge_index], polygon[edge_index + 1]};
}

void forEachEdge(
    nonstd::span<const glm::vec2> polygon,
    const std::function<void(const glm::vec2 &edge_start, const glm::vec2 &edge_end)> &function) {
  const auto edge_count = countEdges(polygon);
  for (size_t index = 0; index < edge_count; ++index) {
    const auto [start, end] = getEdge(polygon, index);
    function(start, end);
  }
}
} // namespace GameEngine::Geometry
