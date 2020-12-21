/** @file
 * Implements a class for representing polygon edges.
 */

#include "PolygonEdge.hpp"
#include "Math.hpp"

namespace GameEngine {
PolygonEdge::PolygonEdge(const glm::vec2 &start, const glm::vec2 &end)
    : start{start}, end{end}, normal{Math::computeNormalOfEdge(start, end)} {}

const glm::vec2 &PolygonEdge::getStart() const { return start; }

const glm::vec2 &PolygonEdge::getEnd() const { return end; }

const glm::vec2 &PolygonEdge::getNormal() const { return normal; }

} // namespace GameEngine
