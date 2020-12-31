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

void GameCharacter::jump() { tick_of_jump_request = current_tick; }

void GameCharacter::accelerate(const GameCharacter::VerticalAcceleration direction) {
  acceleration_direction = direction;
}

bool GameCharacter::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }

const glm::vec2 &GameCharacter::getVelocity() const { return velocity; }

void GameCharacter::update() {
  if (touching_ground) {
    state = GameCharacter::State::TouchingGround;
  } else if (touching_wall) {
    state = GameCharacter::State::TouchingWall;
  } else {
    state = GameCharacter::State::Falling;
  }
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
  if (state == GameCharacter::State::TouchingGround) {
    bounding_polygon.setPosition(bounding_polygon.getPosition() + down);
  } else if (state == GameCharacter::State::TouchingWall) {
    velocity.x = wall_jump_to_right ? -0.5 : 0.5;
    velocity += down * 0.5f;
  } else {
    velocity += down * 0.5f;
  }

  if (jumpScheduled()) {
    if (state == GameCharacter::State::TouchingGround) {
      tick_of_jump_request = 0;
      velocity.y -= 15;
    } else if (state == GameCharacter::State::TouchingWall) {
      tick_of_jump_request = 0;
      const auto inversion_factor = wall_jump_to_right ? 1 : -1;
      const glm::vec2 next_jump_direction = {
          glm::rotate(glm::vec2{0, -1}, glm::radians(45.0f)).x * inversion_factor, -1};
      velocity = next_jump_direction * 15.0f;
    }
  }

  touching_ground = false;
  touching_wall = false;
  right_direction = {1, 0};
}

bool GameCharacter::handleCollisionWith(GameCharacter &, const glm::vec2 &displacement_vector) {
  bounding_polygon.setPosition(bounding_polygon.getPosition() + displacement_vector);

  if (glm::abs(displacement_vector.x) < glm::abs(displacement_vector.y)) {
    /* Vertical collision. */
    const bool character_falls = velocity.y > 0;
    const bool object_below_character = displacement_vector.y < 0;

    if (object_below_character) {
      right_direction = glm::normalize(glm::vec2{-displacement_vector.y, displacement_vector.x});
      velocity = glm::proj(velocity, right_direction);
      touching_ground = true;
    } else if (!character_falls && !object_below_character) {
      velocity.y = 0;
    }
  } else {
    /* Horizontal collision. */
    const bool character_moves_right = velocity.x > 0;
    const bool object_right_of_character = displacement_vector.x < 0;
    if (character_moves_right == object_right_of_character) {
      touching_wall = true;
    }
    wall_jump_to_right = !object_right_of_character;
  }

  return true;
}
} // namespace GameEngine
