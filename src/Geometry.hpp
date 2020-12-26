/** @file
 * Declares geometry helper functions.
 */

#ifndef GAME_ENGINE_SRC_GEOMETRY_HPP
#define GAME_ENGINE_SRC_GEOMETRY_HPP

#include <functional>
#include <glm/vec2.hpp>
#include <nonstd/span.hpp>
#include <optional>
#include <utility>

namespace GameEngine::Geometry {
/** Apply the given function to all edges of specified polygon.
 *
 * @param polygon Represents the polygon which edges should be traversed.
 * @param function Will be called on each edge. Takes the start and end position of the current
 * edge.
 */
void forEachEdge(
    nonstd::span<const glm::vec2> polygon,
    const std::function<void(const glm::vec2 &edge_start, const glm::vec2 &edge_end)> &function);

/** Get the normal vector perpendicular to the given edge.
 *
 * @param edge_start First point of the edge.
 * @param edge_end Last point of the edge.
 *
 * @return Normalized vector perpendicular to the given edge.
 */
glm::vec2 computeNormalOfEdge(const glm::vec2 &edge_start, const glm::vec2 &edge_end);

/** Contains the smallest and largest values found while projecting vertices onto an axis. */
struct ProjectedVertices {
  glm::vec2 axis; /**< Normalized axis onto which vertices got projected. */
  float min;      /**< Smallest projected value. */
  float max;      /**< Largest projected value. */
};

/** Project the given vertices onto the given axis and return the smallest and largest values.
 *
 * @param vertices Points to be projected. Must contain at least one point.
 * @param axis Axis onto which the given points will be projected. Must be normalized.
 *
 * @return Given Axis and the smallest/largest values occurred during projection.
 */
ProjectedVertices projectVerticesOntoAxisMinMax(nonstd::span<const glm::vec2> vertices,
                                                const glm::vec2 &axis);

/** Check if the given polygons collide using the separating axis theorem.
 *
 * @param polygon_a All points of the first polygon. Must be convex.
 * @param polygon_b All points of the second polygon. Must be convex.
 *
 * @return Displacement vector (MTV) for moving polygon_a out of polygon_b. This function will
 * return nothing if no collision was detected.
 */
std::optional<glm::vec2> checkPolygonCollision(nonstd::span<const glm::vec2> polygon_a,
                                               nonstd::span<const glm::vec2> polygon_b);
} // namespace GameEngine::Geometry

#endif
