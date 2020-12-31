/** @file
 * Implements an interactive polygon compatible with the physics engine.
 */

#include "PhysicalObject.hpp"
#include <glm/common.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine {
PhysicalObject::PhysicalObject(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {}

void PhysicalObject::jump() { tick_of_jump_request = current_tick; }

void PhysicalObject::accelerate(const PhysicalObject::VerticalAcceleration direction) {
  acceleration_direction = direction;
}

bool PhysicalObject::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }
} // namespace GameEngine
