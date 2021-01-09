/** @file
 * Implements an interactive character which can run, jump and fall.
 */

#include "Physics/DynamicObject.hpp"
#include <glm/common.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace GameEngine::Physics {
DynamicObject::DynamicObject(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {}

void DynamicObject::update() {
  current_tick++;

  const auto right_direction = getRightDirection();

  const float horizontal_acceleration = ground_normal.has_value() ? 0.05 : 0.025;
  const float ground_friction = ground_normal.has_value() ? 0.05 : 0;
  const float air_friction = 0.01;
  velocity *= 1 - air_friction;
  velocity *= 1 - ground_friction;

  /* Align velocity parallel to ground when moving towards ground. */
  if (ground_normal.has_value() &&
      glm::angle(*ground_normal, glm::normalize(velocity)) > glm::half_pi<float>()) {
    velocity = glm::proj(velocity, right_direction);
  }
  if (is_touching_ceiling) {
    velocity.y = glm::min(velocity.y, 0.0f);
  }
  if (current_sticky_wall_direction != HorizontalDirection::None && !ground_normal.has_value()) {
    const float wall_gravity = horizontal_acceleration * 0.99;
    const float wall_resistance = 0.5;
    const auto x_direction_towards_wall =
        current_sticky_wall_direction == HorizontalDirection::Left ? -1 : 1;
    const bool moving_left = velocity.x < 0;
    const bool wall_is_left = current_sticky_wall_direction == HorizontalDirection::Left;

    if (wall_is_left != moving_left || glm::abs(velocity.x) < wall_gravity) {
      velocity.x += x_direction_towards_wall * wall_gravity;
    }
    velocity.x *= 1 - wall_resistance;
  }

  const auto acceleration_vector =
      acceleration_direction == HorizontalDirection::Left ? -right_direction : right_direction;
  const bool accelerating_in_moving_direction = glm::dot(velocity, acceleration_vector) > 0;
  if (acceleration_direction != HorizontalDirection::None &&
      (glm::length(glm::proj(velocity, acceleration_vector)) < 0.35 ||
       !accelerating_in_moving_direction)) {
    velocity += acceleration_vector * horizontal_acceleration;
  }

  if (jumpScheduled()) {
    const float jump_power = 0.475 * (1 - ground_friction);
    if (ground_normal.has_value()) {
      tick_of_jump_request = 0;
      velocity.y += jump_power;
    } else if (current_sticky_wall_direction != HorizontalDirection::None) {
      tick_of_jump_request = 0;
      const auto inversion_factor =
          current_sticky_wall_direction == HorizontalDirection::Left ? 1 : -1;
      const glm::vec2 next_jump_direction = {
          glm::rotate(glm::vec2{0, 1}, glm::radians(-45.0f)).x * inversion_factor, 1};
      velocity = next_jump_direction * jump_power;
    }
  }

  /* Apply gravity orthogonal to current slope. */
  const float gravity = 0.0125;
  const glm::vec2 down{right_direction.y, -right_direction.x};
  velocity += down * gravity;

  ground_normal.reset();
  current_sticky_wall_direction = HorizontalDirection::None;
  is_touching_ceiling = false;
}

const glm::vec2 &DynamicObject::getVelocity() const { return velocity; }

void DynamicObject::addVelocityOffset(const glm::vec2 &offset) {
  bounding_polygon.setPosition(bounding_polygon.getPosition() + offset);
}

const ConvexBoundingPolygon &DynamicObject::getBoundingPolygon() const { return bounding_polygon; }

void DynamicObject::handleCollisionWith(Physics::Object &, const glm::vec2 &displacement_vector) {
  addVelocityOffset(displacement_vector);

  if (glm::abs(displacement_vector.x) < glm::abs(displacement_vector.y)) {
    /* Vertical collision. */
    const bool character_falls = velocity.y < 0;
    const bool object_below_character = displacement_vector.y > 0;

    if (object_below_character) {
      ground_normal = glm::normalize(displacement_vector);
    } else if (!character_falls) {
      is_touching_ceiling = true;
    }
  } else {
    /* Horizontal collision. */
    const bool character_moves_right = velocity.x > 0;
    const bool object_right_of_character = displacement_vector.x < 0;
    if (character_moves_right == object_right_of_character) {
      current_sticky_wall_direction =
          object_right_of_character ? HorizontalDirection::Right : HorizontalDirection::Left;
    }
  }
}

void DynamicObject::jump() { tick_of_jump_request = current_tick; }

void DynamicObject::accelerate(const DynamicObject::HorizontalDirection direction) {
  acceleration_direction = direction;
}

bool DynamicObject::isTouchingGround() const { return ground_normal.has_value(); }

bool DynamicObject::isTouchingWall() const {
  return current_sticky_wall_direction != HorizontalDirection::None;
}

glm::vec2 DynamicObject::getRightDirection() const {
  if (!ground_normal.has_value()) {
    /* Fall back to X axis. */
    return {1, 0};
  }
  return {ground_normal->y, -ground_normal->x};
}

bool DynamicObject::jumpScheduled() const {
  const auto ticks_tolerance = 6;
  return current_tick - tick_of_jump_request < ticks_tolerance;
}
} // namespace GameEngine::Physics
