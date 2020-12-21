/** @file
 * Implements a class for representing polygon edges.
 */

#include "PolygonEdge.hpp"
#include <glm/geometric.hpp>

namespace GameEngine {
PolygonEdge::PolygonEdge(const glm::vec2 &start, const glm::vec2 &end)
    : start{start}, end{end}, normal{glm::normalize(glm::vec2{start.y - end.y, end.x - start.x})} {}

const glm::vec2 &PolygonEdge::getStart() const { return start; }

const glm::vec2 &PolygonEdge::getEnd() const { return end; }

const glm::vec2 &PolygonEdge::getNormal() const { return normal; }

} // namespace GameEngine
