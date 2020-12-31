/** @file
 * Implements an interactive polygon compatible with the physics engine.
 */

#include "PhysicalObject.hpp"
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
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
  if (!physics_enabled) {
    return;
  }

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
  const auto gravity = glm::vec2{-right_direction.y, right_direction.x} * 0.5f;
  velocity += gravity;
  bounding_polygon.setPosition(bounding_polygon.getPosition() + velocity);
  current_tick++;
}

void PhysicalObject::handleCollision(PhysicalObject &, const glm::vec2 &displacement_vector) {
  if (!physics_enabled) {
    return;
  }
  bounding_polygon.setPosition(bounding_polygon.getPosition() + displacement_vector);

  if (glm::abs(displacement_vector.x) < glm::abs(displacement_vector.y)) {
    /* Vertical collision. */
    const bool character_falls = velocity.y > 0;
    const bool object_below_character = displacement_vector.y < 0;

    if (character_falls && object_below_character) {
      right_direction = glm::normalize(glm::vec2{-displacement_vector.y, displacement_vector.x});
      velocity = glm::proj(velocity, right_direction);
      tick_of_last_floor_collision = current_tick;
    } else if (!character_falls && !object_below_character) {
      velocity.y = 0;
    }
  } else {
    /* Horizontal collision. */
    const bool character_moves_right = velocity.x > 0;
    const bool object_right_of_character = displacement_vector.x < 0;
    if (character_moves_right == object_right_of_character) {
      velocity.x = 0;
      tick_of_last_wall_collision = current_tick;
    }
    wall_jump_to_right = !object_right_of_character;
  }
}

bool PhysicalObject::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }
bool PhysicalObject::sticksToFloor() const {
  return current_tick - tick_of_last_floor_collision < 1;
}
bool PhysicalObject::sticksToWall() const { return current_tick - tick_of_last_wall_collision < 6; }
} // namespace GameEngine
