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
/** Represents a convex polygon used for collision detection. */
using ConvexPolygonView = nonstd::span<const glm::vec2>;

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

/** Compute the center of the given polygon.
 *
 * @param polygon Convex polygon with at least one vertex.
 *
 * @return Center coordinates of the given convex polygon.
 */
glm::vec2 computeCenter(ConvexPolygonView polygon);

/** Check if the given polygons collide using the separating axis theorem.
 *
 * @param a All points of the first polygon. Must be convex.
 * @param b All points of the second polygon. Must be convex.
 *
 * @return Displacement vector (MTV) for moving polygon a out of polygon b. This function will
 * return nothing if no collision occurred.
 *
 * Usage examples:
 *
 * @code
 * std::array my_triangle{
 *     glm::vec2{575, 400},
 *     glm::vec2{792, 515},
 *     glm::vec2{870, 670},
 * };
 *
 * std::array my_line{
 *     glm::vec2{575, 400},
 *     glm::vec2{870, 670},
 * };
 *
 * if(checkCollision(my_triangle, my_line) {
 *   ...
 * }
 * @endcode
 */
std::optional<glm::vec2> checkCollision(ConvexPolygonView a, ConvexPolygonView b);
} // namespace GameEngine::Geometry

#endif
