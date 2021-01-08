/** @file
 * Contains a camera class.
 */

#ifndef GAME_ENGINE_SRC_CAMERA_HPP
#define GAME_ENGINE_SRC_CAMERA_HPP

#include <cstddef>
#include <glm/vec2.hpp>

namespace GameEngine {
/** Transforms game world coordinates to screen coordinates. The screens aspect ratio will be
 * considered. E.g. a 16:9 monitor spans 48x27 units in the game world. */
class Camera {
public:
  Camera(size_t screen_width, size_t screen_height);

  glm::vec2 toScreenCoordinate(const glm::vec2 &world_coordinate) const;
  glm::vec2 toWorldCoordinate(const glm::vec2 &screen_coordinate) const;

  /** @param position New center of the camera in the game world. */
  void setPosition(const glm::vec2 &position);

  /** @param scaling_factor E.g. 2.0f to zoom in by 2x. Defaults to 1.0f. */
  void setScale(float scaling_factor);

  /** @param orientation Rotation angle in radians. */
  void setOrientation(float orientation);

private:
  glm::vec2 screen_center;

  /** Factor for scaling world coordinates up to screen coordinates. */
  glm::vec2 world_to_screen_scaling_factor;

  glm::vec2 position = {0, 0};
  float scaling_factor = 1;
  float orientation = 0;
};
} // namespace GameEngine

#endif
