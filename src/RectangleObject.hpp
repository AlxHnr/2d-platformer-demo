/** @file
 * Declares an interactive rectangle.
 */

#ifndef GAME_ENGINE_SRC_RECTANGLE_OBJECT_HPP
#define GAME_ENGINE_SRC_RECTANGLE_OBJECT_HPP

#include "PhysicsObject.hpp"

namespace GameEngine {
/** Basic rectangle to which physics and collisions will be applied. */
class RectangleObject : public PhysicsObject {
public:
  RectangleObject(const glm::vec2 &center_position, float width, float heigth);

  /** @param angle Rotation value in radians, clockwise direction. */
  void rotate(float angle);

  /* PhysicsObject interface functions. */
  const glm::vec2 &getPosition() override;
  void setPosition(const glm::vec2 &new_position) override;
  const glm::vec2 &getVelocity() override;
  void setVelocity(const glm::vec2 &new_velocity) override;
  Geometry::ConvexPolygonView getBoundingPolygon() override;

private:
  /** Center of this object. */
  glm::vec2 position;

  /** Clockwise rotation angle in radians. */
  float angle{0.0f};

  /** Speed and direction of this object. */
  glm::vec2 velocity{0.0f, 0.0f};

  /** Width of the rectangle. */
  float width;

  /** Height of the rectangle. */
  float height;

  /** Position of the objects borders the game world used for collision detection. */
  std::array<glm::vec2, 4> bounding_box;

  void recomputeBoundingBox();
};
} // namespace GameEngine

#endif
