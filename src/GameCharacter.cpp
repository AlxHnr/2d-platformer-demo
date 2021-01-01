/** @file
 * Implements an interactive character which can run, jump and fall.
 */

#include "GameCharacter.hpp"
#include <glm/common.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine {
GameCharacter::GameCharacter(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {}

void GameCharacter::update() {
  current_tick++;

  const auto right_direction = getRightDirection();
  if (ground_normal.has_value()) {
    velocity = glm::proj(velocity, right_direction);
  }

  const auto acceleration_vector =
      acceleration_direction == GameCharacter::VerticalAcceleration::Left ? -right_direction
                                                                          : right_direction;
  const bool accelerating_in_moving_direction = glm::dot(velocity, acceleration_vector) > 0;
  if (acceleration_direction == GameCharacter::VerticalAcceleration::None) {
    const glm::vec2 friction_factor{0.95, 1};
    velocity *= friction_factor;
  } else if (!accelerating_in_moving_direction ||
             glm::length(glm::proj(velocity, acceleration_vector)) < 10.0) {
    velocity += acceleration_vector;
  }

  /* Apply gravity perpendicular to current slope. */
  const float gravity = 0.5;
  const glm::vec2 down{-right_direction.y, right_direction.x};
  if (is_touching_ceiling) {
    velocity.y = 0;
  }
  if (is_touching_wall) {
    velocity.x = wall_jump_to_right ? -gravity : gravity;
  }
  velocity += down * gravity;

  if (jumpScheduled()) {
    if (ground_normal.has_value()) {
      tick_of_jump_request = 0;
      velocity.y -= 15;
    } else if (is_touching_wall) {
      tick_of_jump_request = 0;
      const auto inversion_factor = wall_jump_to_right ? 1 : -1;
      const glm::vec2 next_jump_direction = {
          glm::rotate(glm::vec2{0, -1}, glm::radians(45.0f)).x * inversion_factor, -1};
      velocity = next_jump_direction * 15.0f;
    }
  }

  ground_normal.reset();
  is_touching_wall = false;
  is_touching_ceiling = false;
}

const glm::vec2 &GameCharacter::getVelocity() const { return velocity; }

void GameCharacter::addVelocityOffset(const glm::vec2 &offset) {
  bounding_polygon.setPosition(bounding_polygon.getPosition() + offset);
}

const ConvexBoundingPolygon &GameCharacter::getBoundingPolygon() const { return bounding_polygon; }

bool GameCharacter::handleCollisionWith(PhysicalObject &, const glm::vec2 &displacement_vector) {
  addVelocityOffset(displacement_vector);

  if (glm::abs(displacement_vector.x) < glm::abs(displacement_vector.y)) {
    /* Vertical collision. */
    const bool character_falls = velocity.y > 0;
    const bool object_below_character = displacement_vector.y < 0;

    if (object_below_character) {
      ground_normal = glm::normalize(displacement_vector);
    } else if (!character_falls && !object_below_character) {
      is_touching_ceiling = true;
    }
  } else {
    /* Horizontal collision. */
    const bool character_moves_right = velocity.x > 0;
    const bool object_right_of_character = displacement_vector.x < 0;
    if (character_moves_right == object_right_of_character) {
      is_touching_wall = true;
    }
    wall_jump_to_right = !object_right_of_character;
  }

  return false;
}

void GameCharacter::jump() { tick_of_jump_request = current_tick; }

void GameCharacter::accelerate(const GameCharacter::VerticalAcceleration direction) {
  acceleration_direction = direction;
}

bool GameCharacter::isTouchingGround() const { return ground_normal.has_value(); }

bool GameCharacter::isTouchingWall() const { return is_touching_wall; }

const glm::vec2 GameCharacter::getRightDirection() const {
  if (!ground_normal.has_value()) {
    /* Fall back to X axis. */
    return {1, 0};
  }
  return {-ground_normal->y, ground_normal->x};
}

bool GameCharacter::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }
} // namespace GameEngine
