/** @file
 * Declares math helper functions.
 */

#ifndef GAME_ENGINE_SRC_MATH_HPP
#define GAME_ENGINE_SRC_MATH_HPP

#include <functional>
#include <glm/vec2.hpp>
#include <nonstd/span.hpp>
#include <utility>

namespace GameEngine::Math {
/** Apply the given function to all edges of specified polygon.
 *
 * @param vertices Represents the polygon which edges should be traversed.
 * @param function Will be called on each edge. Takes three parameters: Index of the current edge,
 * start position of the current edge end end position of the current edge.
 */
void forEachEdge(nonstd::span<const glm::vec2> vertices,
                 const std::function<void(const size_t edge_index, const glm::vec2 &edge_start,
                                          const glm::vec2 &edge_end)> &function);

/** Get the normal vector perpendicular to the given edge.
 *
 * @param edge_start First point of the edge.
 * @param edge_end Last point of the edge.
 *
 * @return Vector perpendicular to the given edge.
 */
glm::vec2 computeNormalOfEdge(const glm::vec2 &edge_start, const glm::vec2 &edge_end);

/** Project the given vertices onto the given axis and return the smallest and largest values.
 *
 * @param vertices Points to be projected.
 * @param axis Given points will be projected on this axis.
 *
 * @return [min, max] Smallest and largest values occurred during projection.
 */
std::pair<float, float> projectVerticesOntoAxisMinMax(nonstd::span<const glm::vec2> vertices,
                                                      const glm::vec2 &axis);

/** Contains the smallest and largest values found while projecting arbitrary vertices onto an
 * axis. */
struct ProjectetVerticesMinMax {
  glm::vec2 axis;
  float min; /**< Smallest projected value. */
  float max; /**< Largest projected value. */
};
} // namespace GameEngine::Math

#endif
