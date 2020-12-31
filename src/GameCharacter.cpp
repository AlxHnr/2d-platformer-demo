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
} // namespace GameEngine
