/** @file
 * Declares geometry helper functions.
 */

#ifndef GAME_ENGINE_SRC_GEOMETRY_HPP
#define GAME_ENGINE_SRC_GEOMETRY_HPP

#include <functional>
#include <glm/vec2.hpp>
#include <nonstd/span.hpp>
#include <optional>

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

/** Check if the given polygons collide using the separating axis theorem.
 *
 * @param polygon_a All points of the first polygon. Must be convex.
 * @param polygon_b All points of the second polygon. Must be convex.
 *
 * @return Displacement vector (MTV) for moving polygon_a out of polygon_b. This function will
 * return nothing if no collision occurred.
 */
std::optional<glm::vec2> checkCollision(nonstd::span<const glm::vec2> polygon_a,
                                        nonstd::span<const glm::vec2> polygon_b);
} // namespace GameEngine::Geometry

#endif
