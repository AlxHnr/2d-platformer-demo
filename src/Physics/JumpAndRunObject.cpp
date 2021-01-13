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
  using namespace std::chrono_literals;

  /* Capture base classes values before calling its update() function. */
  const bool standing_on_ground = isTouchingGround();
  const auto direction_to_colliding_wall = isTouchingWall();
  const auto right_direction = getRightDirection();
  if (direction_to_colliding_wall.has_value() && getVelocity().y < 0) {
    setVelocity(getVelocity() * (1 - wall_grip));
  }
  DynamicObject::update();

  if (standing_on_ground) {
    airjumps_remaining = airjumps_max;
  }

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

  if (std::chrono::steady_clock::now() - time_of_jump_request < 100ms) {
    if (standing_on_ground) {
      time_of_jump_request = {};
      setVelocity({getVelocity().x, jump_power * (1 - getGroundStickiness())});
    } else if (walljump_enabled && direction_to_colliding_wall.has_value()) {
      time_of_jump_request = {};
      const auto inversion_factor =
          direction_to_colliding_wall == HorizontalDirection::Left ? -1 : 1;
      const glm::vec2 jump_direction = {
          glm::rotate(glm::vec2{0, 1}, glm::radians(45.0f)).x * inversion_factor, 1};
      setVelocity(jump_direction * jump_power * (1 - getWallStickiness()));
    } else if (airjumps_remaining > 0) {
      time_of_jump_request = {};
      airjumps_remaining--;
      setVelocity({getVelocity().x, jump_power});
    }
  }
}

void JumpAndRunObject::jump() { time_of_jump_request = std::chrono::steady_clock::now(); }

void JumpAndRunObject::run(const std::optional<HorizontalDirection> direction) {
  acceleration_direction = direction;
}

float JumpAndRunObject::getJumpPower() const { return jump_power; }

void JumpAndRunObject::setJumpPower(const float jump_power) {
  this->jump_power = glm::max(jump_power, 0.0f);
}

bool JumpAndRunObject::getWalljumpEnabled() const { return walljump_enabled; }

void JumpAndRunObject::setWalljumpEnabled(const bool enabled) { walljump_enabled = enabled; }

float JumpAndRunObject::getWallGrip() const { return wall_grip; }

void JumpAndRunObject::setWallGrip(const float wall_grip) {
  this->wall_grip = glm::clamp(wall_grip, 0.0f, 1.0f);
}

uint16_t JumpAndRunObject::getAirjumpsMax() const { return airjumps_max; }

void JumpAndRunObject::setAirjumpsMax(const uint16_t airjumps_max) {
  this->airjumps_max = airjumps_max;
}

float JumpAndRunObject::getHorizontalAcceleration() const { return horizontal_acceleration; }

void JumpAndRunObject::setHorizontalAcceleration(const float horizontal_acceleration) {
  this->horizontal_acceleration = glm::max(horizontal_acceleration, 0.0f);
}

float JumpAndRunObject::getHorizontalSpeedMax() const { return horizontal_speed_max; }

void JumpAndRunObject::setHorizontalSpeedMax(const float horizontal_speed_max) {
  this->horizontal_speed_max = glm::max(horizontal_speed_max, 0.0f);
}
} // namespace GameEngine::Physics
