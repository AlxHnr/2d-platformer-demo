/** @file
 * Contains a camera class.
 */

#ifndef GAME_ENGINE_INCLUDE_GAME_ENGINE_CAMERA_HPP
#define GAME_ENGINE_INCLUDE_GAME_ENGINE_CAMERA_HPP

#include <cstddef>
#include <glm/vec2.hpp>

namespace GameEngine {
/** Transforms game world coordinates to screen coordinates. The screens aspect ratio will be
 * considered. E.g. a 16:9 monitor spans 48x27 units in the game world. */
class Camera {
public:
  Camera(size_t screen_width, size_t screen_height);

  glm::vec2 toScreenCoordinate(glm::vec2 world_coordinate) const;
  glm::vec2 toWorldCoordinate(glm::vec2 screen_coordinate) const;

  /** @param position New center of the camera in the game world. */
  void setPosition(glm::vec2 position);

  /** @param zoom E.g. 2.0f to zoom in by 2x. Defaults to 1.0f. */
  void setZoom(float zoom);

  /** @param orientation Rotation angle in radians. */
  void setOrientation(float orientation);

  /** Perform a single step towards the given target position using averaging and interpolation.
   * Should be called every frame. */
  void stepTowardsPosition(glm::vec2 target_position);

private:
  glm::vec2 position = {0, 0};
  float zoom = 1;
  float orientation = 0;

  glm::vec2 center_on_screen;
  glm::vec2 world_to_screen_scaling_factor;
};
} // namespace GameEngine

#endif
