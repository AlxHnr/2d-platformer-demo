/** @file
 * Declares geometry helper functions.
 */

#ifndef GAME_ENGINE_SRC_GEOMETRY_HPP
#define GAME_ENGINE_SRC_GEOMETRY_HPP

#include <functional>
#include <glm/vec2.hpp>
#include <nonstd/span.hpp>

namespace GameEngine::Geometry {
/** Count the edges of the given polygon.
 *
 * @param polygon Contains zero or more vertices.
 *
 * @return Amount of edges in the given polygon. E.g. a triangle with 3 points has 3 edges. A line
 * with 2 points has one edge.
 */
size_t countEdges(nonstd::span<const glm::vec2> polygon);

/** Get the edge from the given polygon specified by the edges index.
 *
 * @param polygon Contains at least two vertices representing an edge.
 * @param edge_index Index of the edge starting at 0. Must be inside the bounds of the given
 * polygon. See countEdges().
 *
 * @return [start, end] positions of the polygons nth edge.
 */
std::pair<glm::vec2, glm::vec2> getEdge(nonstd::span<const glm::vec2> polygon,
                                        const size_t edge_index);

/** Apply the given function to all edges of specified polygon.
 *
 * @param polygon Represents the polygon which edges should be traversed.
 * @param function Will be called on each edge. Takes the start and end position of the current
 * edge.
 */
void forEachEdge(
    nonstd::span<const glm::vec2> polygon,
    const std::function<void(const glm::vec2 &edge_start, const glm::vec2 &edge_end)> &function);
} // namespace GameEngine::Geometry

#endif
