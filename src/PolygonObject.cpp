/** @file
 * Implements an interactive polygon compatible with the physics engine.
 */

#include "PolygonObject.hpp"
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine {
PolygonObject::PolygonObject(std::initializer_list<glm::vec2> vertices) : vertices{vertices} {
  position = vertices.size() == 0 ? glm::vec2{0, 0} : Geometry::computeCenter(vertices);
  std::transform(vertices.begin(), vertices.end(), std::back_inserter(vertices_relative_to_center),
                 [this](const glm::vec2 &vertex) { return vertex - position; });
}

void PolygonObject::rotate(const float angle) {
  orientation = glm::mod(orientation + angle, glm::pi<float>() * 2);
  recomputeBoundingBox();
}

const glm::vec2 &PolygonObject::getPosition() const { return position; }

void PolygonObject::setPosition(const glm::vec2 &new_position) {
  position = new_position;
  recomputeBoundingBox();
}

const glm::vec2 &PolygonObject::getVelocity() const { return velocity; }

void PolygonObject::setVelocity(const glm::vec2 &new_velocity) { velocity = new_velocity; }

Geometry::ConvexPolygonView PolygonObject::getBoundingPolygon() const { return vertices; }

void PolygonObject::recomputeBoundingBox() {
  std::transform(
      vertices_relative_to_center.cbegin(), vertices_relative_to_center.cend(), vertices.begin(),
      [this](const glm::vec2 &vertex) { return glm::rotate(vertex, orientation) + position; });
}
} // namespace GameEngine
