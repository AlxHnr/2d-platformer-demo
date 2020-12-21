/** @file
 * Contains a class for representing polygon edges.
 */

#ifndef GAME_ENGINE_SRC_POLYGON_EDGE_HPP
#define GAME_ENGINE_SRC_POLYGON_EDGE_HPP

#include <glm/vec2.hpp>

namespace GameEngine {
/** Represents an edge of a polygon. */
class PolygonEdge {
public:
  PolygonEdge() = default;
  PolygonEdge(const glm::vec2 &start, const glm::vec2 &end);
  const glm::vec2 &getStart() const;
  const glm::vec2 &getEnd() const;
  const glm::vec2 &getNormal() const;

private:
  glm::vec2 start;
  glm::vec2 end;
  glm::vec2 normal;
};
} // namespace GameEngine

#endif
