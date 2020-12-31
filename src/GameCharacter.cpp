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
  glm::vec2 down{-right_direction.y, right_direction.x};
  if (is_touching_ground) {
    bounding_polygon.setPosition(bounding_polygon.getPosition() + down);
  } else if (is_touching_wall) {
    velocity.x = wall_jump_to_right ? -0.5 : 0.5;
    velocity += down * 0.5f;
  } else {
    velocity += down * 0.5f;
  }

  if (jumpScheduled()) {
    if (is_touching_ground) {
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

  is_touching_ground = false;
  is_touching_wall = false;
  right_direction = {1, 0};
}

const glm::vec2 &GameCharacter::getVelocity() const { return velocity; }

void GameCharacter::addVelocityOffset(const glm::vec2 &offset) {
  bounding_polygon.setPosition(bounding_polygon.getPosition() + offset);
}

const ConvexBoundingPolygon &GameCharacter::getBoundingPolygon() const { return bounding_polygon; }

bool GameCharacter::handleCollisionWith(PhysicalObject &, const glm::vec2 &displacement_vector) {
  bounding_polygon.setPosition(bounding_polygon.getPosition() + displacement_vector);

  if (glm::abs(displacement_vector.x) < glm::abs(displacement_vector.y)) {
    /* Vertical collision. */
    const bool character_falls = velocity.y > 0;
    const bool object_below_character = displacement_vector.y < 0;

    if (object_below_character) {
      right_direction = glm::normalize(glm::vec2{-displacement_vector.y, displacement_vector.x});
      velocity = glm::proj(velocity, right_direction);
      is_touching_ground = true;
    } else if (!character_falls && !object_below_character) {
      velocity.y = 0;
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

  return true;
}

void GameCharacter::jump() { tick_of_jump_request = current_tick; }

void GameCharacter::accelerate(const GameCharacter::VerticalAcceleration direction) {
  acceleration_direction = direction;
}

bool GameCharacter::isTouchingGround() const { return is_touching_ground; }

bool GameCharacter::isTouchingWall() const { return is_touching_wall; }

const glm::vec2 &GameCharacter::getRightDirection() const { return right_direction; }

bool GameCharacter::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }
} // namespace GameEngine
