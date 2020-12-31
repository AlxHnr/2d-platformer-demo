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

void PhysicalObject::accelerate(const PhysicalObject::Acceleration direction) {
  acceleration_direction = direction;
}

void PhysicalObject::update() {
  if (jumpScheduled()) {
    if (sticksToFloor()) {
      tick_of_jump_request = 0;
      tick_of_last_floor_collision = 0;
      tick_of_last_wall_collision = 0;
      velocity.y -= 15;
    } else if (sticksToWall()) {
      tick_of_jump_request = 0;
      tick_of_last_floor_collision = 0;
      tick_of_last_wall_collision = 0;
      const auto inversion_factor = wall_jump_to_right ? 1 : -1;
      const glm::vec2 next_jump_direction = {
          glm::rotate(glm::vec2{0, -1}, glm::radians(45.0f)).x * inversion_factor, -1};
      velocity = next_jump_direction * 15.0f;
    }
  }

  if (!sticksToFloor()) {
    right_direction = {1, 0};
  }

  const auto acceleration_vector =
      acceleration_direction == Acceleration::Left ? -right_direction : right_direction;
  const bool accelerating_in_moving_direction = glm::dot(velocity, acceleration_vector) > 0;
  if (acceleration_direction == Acceleration::None) {
    const glm::vec2 friction_factor{0.95, 1};
    velocity *= friction_factor;
  } else if (!accelerating_in_moving_direction ||
             glm::length(glm::proj(velocity, acceleration_vector)) < 10.0) {
    velocity += acceleration_vector;
  }

  /* Apply gravity perpendicular to current slope. */
  glm::vec2 down{-right_direction.y, right_direction.x};
  if (state == State::StuckToGround) {
    bounding_polygon.setPosition(bounding_polygon.getPosition() + down);
  } else {
    velocity += down * 0.5f;
  }
  bounding_polygon.setPosition(bounding_polygon.getPosition() + velocity);
  current_tick++;
}

bool PhysicalObject::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }
bool PhysicalObject::sticksToFloor() const {
  return current_tick - tick_of_last_floor_collision < 1;
}
bool PhysicalObject::sticksToWall() const { return current_tick - tick_of_last_wall_collision < 6; }
} // namespace GameEngine
