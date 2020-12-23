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
 * @return Normalized vector perpendicular to the given edge.
 */
glm::vec2 computeNormalOfEdge(const glm::vec2 &edge_start, const glm::vec2 &edge_end);

/** Project the given vertices onto the given axis and return the smallest and largest values.
 *
 * @param vertices Points to be projected.
 * @param axis Normalized axis onto which the given points will be projected.
 *
 * @return [min, max] Smallest and largest values occurred during projection.
 */
std::pair<float, float> projectVerticesOntoAxisMinMax(nonstd::span<const glm::vec2> vertices,
                                                      const glm::vec2 &axis);

/** Contains the smallest and largest values found while projecting vertices onto an axis. */
struct ProjectetVerticesMinMax {
  glm::vec2 axis; /**< Normalized axis onto which vertices got projected. */
  float min;      /**< Smallest projected value. */
  float max;      /**< Largest projected value. */
};

/** Check if the given convex polygons collide using the separating axis theorem.
 *
 * @param polygon_a All points of the first polygon.
 * @param polygon_a_minmax Contains all normal axes of the first polygon and the minmax values
 * resulting from projecting all points of the first polygon onto these axes.
 * @param polygon_b All points of the second polygon.
 * @param polygon_b_minmax Contains all normal axes of the second polygon and the minmax values
 * resulting from projecting all points of the second polygon onto these axes.
 *
 * @return Displacement vector (MTV) for moving polygon_a out of polygon_b. This function will
 * return nothing if no collision was detected.
 */
std::optional<glm::vec2>
checkPolygonCollision(nonstd::span<const glm::vec2> polygon_a,
                      nonstd::span<const ProjectetVerticesMinMax> polygon_a_minmax,
                      nonstd::span<const glm::vec2> polygon_b,
                      nonstd::span<const ProjectetVerticesMinMax> polygon_b_minmax);
} // namespace GameEngine::Geometry

#endif
