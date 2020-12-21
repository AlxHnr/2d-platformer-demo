/** @file
 * Declares math helper functions.
 */

#ifndef GAME_ENGINE_SRC_MATH_HPP
#define GAME_ENGINE_SRC_MATH_HPP

#include <glm/vec2.hpp>
#include <nonstd/span.hpp>
#include <utility>

namespace GameEngine::Math {
/** Get the normal vector perpendicular to the given edge.
 *
 * @param edge_start First point of the edge.
 * @param edge_end Last point of the edge.
 *
 * @return Vector perpendicular to the given edge.
 */
glm::vec2 computeNormalOfEdge(const glm::vec2 &edge_start, const glm::vec2 &edge_end);

/** Project the given vertices onto the given axis and return the smallest and biggest values.
 *
 * @param vertices Points to be projected.
 * @param axis Given points will be projected on this axis.
 *
 * @return [min, max] Smallest and biggest values occurred during projection.
 */
std::pair<float, float> projectVerticesOntoAxisMinMax(nonstd::span<const glm::vec2> vertices,
                                                      const glm::vec2 &axis);
} // namespace GameEngine::Math

#endif
