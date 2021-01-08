/** @file
 * Implements a camera for transforming world coordinates to screen coordinates.
 */

#include "Camera.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace {
/* Width/height of the camera in the game world on a square monitor. */
constexpr float game_world_camera_side_length = 27;
} // namespace

namespace GameEngine {
Camera::Camera(const size_t screen_width, const size_t screen_height)
    : screen_center{screen_width / 2, screen_height / 2} {
  const float scaling_factor =
      glm::min(screen_width, screen_height) / game_world_camera_side_length;
  world_to_screen_scale = {scaling_factor, -scaling_factor};
}

glm::vec2 Camera::toScreenCoordinate(const glm::vec2 &world_coordinate) const {
  return glm::rotate((world_coordinate - position) * world_to_screen_scale * zoom, orientation) +
         screen_center;
}

glm::vec2 Camera::toWorldCoordinate(const glm::vec2 &screen_coordinate) const {
  return glm::rotate(screen_coordinate - screen_center, -orientation) / zoom /
             world_to_screen_scale +
         position;
}

void Camera::setPosition(const glm::vec2 &position) { this->position = position; }

void Camera::setScale(const float scaling_factor) { this->zoom = glm::max(scaling_factor, 0.0f); }

void Camera::setOrientation(const float orientation) {
  this->orientation = glm::mod(orientation, glm::two_pi<float>());
}
} // namespace GameEngine
