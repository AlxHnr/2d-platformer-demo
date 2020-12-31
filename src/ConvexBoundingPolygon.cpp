/** @file
 * Implements a basic polygon class.
 */

#include "ConvexBoundingPolygon.hpp"
#include "Geometry.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine {
ConvexBoundingPolygon::ConvexBoundingPolygon(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {
  position = vertices.size() == 0 ? glm::vec2{0, 0} : Geometry::computeCenter(vertices);
  std::transform(vertices.begin(), vertices.end(),
                 std::back_inserter(bounding_polygon_relative_to_center),
                 [this](const glm::vec2 &vertex) { return vertex - position; });
}

const glm::vec2 &ConvexBoundingPolygon::getPosition() const { return position; }

void ConvexBoundingPolygon::setPosition(const glm::vec2 &position) {
  this->position = position;
  recomputeBoundingPolygon();
}

float ConvexBoundingPolygon::getOrientation() const { return orientation; }

void ConvexBoundingPolygon::setOrientation(float orientation) {
  this->orientation = glm::mod(orientation, glm::two_pi<float>());
  recomputeBoundingPolygon();
}

void ConvexBoundingPolygon::forEachEdge(
    const std::function<void(const glm::vec2 &edge_start, const glm::vec2 &edge_end)> &function)
    const {
  Geometry::forEachEdge(bounding_polygon, function);
}

std::optional<glm::vec2>
ConvexBoundingPolygon::collidesWith(const ConvexBoundingPolygon &other) const {
  return Geometry::checkCollision(bounding_polygon, other.bounding_polygon);
}

void ConvexBoundingPolygon::recomputeBoundingPolygon() {
  std::transform(
      bounding_polygon_relative_to_center.cbegin(), bounding_polygon_relative_to_center.cend(),
      bounding_polygon.begin(),
      [this](const glm::vec2 &vertex) { return glm::rotate(vertex, orientation) + position; });
}
} // namespace GameEngine
