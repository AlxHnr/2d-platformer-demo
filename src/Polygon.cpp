/** @file
 * Implements a basic polygon class.
 */

#include "Polygon.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine {
Polygon::Polygon(std::initializer_list<glm::vec2> vertices) : bounding_polygon{vertices} {
  position = vertices.size() == 0 ? glm::vec2{0, 0} : Geometry::computeCenter(vertices);
  std::transform(vertices.begin(), vertices.end(),
                 std::back_inserter(bounding_polygon_relative_to_center),
                 [this](const glm::vec2 &vertex) { return vertex - position; });
}

Geometry::ConvexPolygonView Polygon::getBoundingPolygon() const { return bounding_polygon; }

const glm::vec2 &Polygon::getPosition() const { return position; }

void Polygon::setPosition(const glm::vec2 &position) {
  this->position = position;
  recomputeBoundingPolygon();
}

float Polygon::getOrientation() const { return orientation; }

void Polygon::setOrientation(float orientation) {
  this->orientation = glm::mod(orientation, glm::two_pi<float>());
  recomputeBoundingPolygon();
}

void Polygon::recomputeBoundingPolygon() {
  std::transform(
      bounding_polygon_relative_to_center.cbegin(), bounding_polygon_relative_to_center.cend(),
      bounding_polygon.begin(),
      [this](const glm::vec2 &vertex) { return glm::rotate(vertex, orientation) + position; });
}
} // namespace GameEngine
