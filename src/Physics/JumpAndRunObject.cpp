/** @file
 * Implements an object with jump-and-run mechanics.
 */

#include "Physics/JumpAndRunObject.hpp"
#include <glm/common.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine::Physics {
JumpAndRunObject::JumpAndRunObject(std::initializer_list<glm::vec2> vertices)
    : DynamicObject{vertices} {}

void JumpAndRunObject::update() {
  /* Capture base classes values before calling its update() function. */
  const bool standing_on_ground = isTouchingGround();
  const auto direction_to_colliding_wall = isTouchingWall();
  const auto right_direction = getRightDirection();
  DynamicObject::update();

  if (acceleration_direction.has_value()) {
    const auto acceleration_vector =
        acceleration_direction == HorizontalDirection::Left ? -right_direction : right_direction;
    const bool accelerating_in_moving_direction = glm::dot(getVelocity(), acceleration_vector) > 0;
    if (glm::length(glm::proj(getVelocity(), acceleration_vector)) < horizontal_speed_max ||
        !accelerating_in_moving_direction) {
      const float factor = standing_on_ground ? 1 : 0.5;
      setVelocity(getVelocity() + acceleration_vector * horizontal_acceleration * factor);
    }
  }

  current_tick++;
  const auto ticks_tolerance = 6;
  if (current_tick - tick_of_jump_request < ticks_tolerance) {
    if (standing_on_ground) {
      tick_of_jump_request = 0;
      setVelocity(getVelocity() + glm::vec2{0, jump_power});
    } else if (direction_to_colliding_wall.has_value()) {
      tick_of_jump_request = 0;
      const auto inversion_factor =
          direction_to_colliding_wall == HorizontalDirection::Left ? -1 : 1;
      const glm::vec2 jump_direction = {
          glm::rotate(glm::vec2{0, 1}, glm::radians(45.0f)).x * inversion_factor, 1};
      setVelocity(jump_direction * jump_power);
    }
  }
}

void JumpAndRunObject::jump() { tick_of_jump_request = current_tick; }

void JumpAndRunObject::run(const std::optional<HorizontalDirection> direction) {
  acceleration_direction = direction;
}
} // namespace GameEngine::Physics
