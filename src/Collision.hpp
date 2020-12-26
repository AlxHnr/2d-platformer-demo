/** @file
 * Helpers for collision handling.
 */

#ifndef GAME_ENGINE_SRC_COLLISION_HPP
#define GAME_ENGINE_SRC_COLLISION_HPP

#include <glm/vec2.hpp>
#include <nonstd/span.hpp>
#include <optional>

namespace GameEngine {
/** Represents a convex polygon used for collision detection. */
using ConvexPolygonView = nonstd::span<const glm::vec2>;

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
} // namespace GameEngine

#endif
