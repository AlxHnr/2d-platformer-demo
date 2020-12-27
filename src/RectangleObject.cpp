/** @file
 * Implements an interactive rectangle compatible with the physics engine.
 */

#include "RectangleObject.hpp"
#include <SDL_assert.h>
#include <glm/common.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine {
RectangleObject::RectangleObject(const glm::vec2 &center_position, const float width,
                                 const float height)
    : position{center_position}, width{width}, height{height} {
  SDL_assert(width > 0);
  SDL_assert(height > 0);
  recomputeBoundingBox();
}

void RectangleObject::rotate(const float degree) {
  angle = glm::clamp(angle + degree, 0.0f, 360.0f);
  recomputeBoundingBox();
}

const glm::vec2 &RectangleObject::getPosition() { return position; }

void RectangleObject::setPosition(const glm::vec2 &new_position) {
  position = new_position;
  recomputeBoundingBox();
}

const glm::vec2 &RectangleObject::getVelocity() { return velocity; }

void RectangleObject::setVelocity(const glm::vec2 &new_velocity) { velocity = new_velocity; }

ConvexPolygonView RectangleObject::getBoundingPolygon() { return bounding_box; }

void RectangleObject::recomputeBoundingBox() {
  const auto half_width = width / 2;
  const auto half_height = height / 2;

  bounding_box = {
      glm::rotate(glm::vec2{-half_width, half_height}, angle) + position,
      glm::rotate(glm::vec2{-half_width, -half_height}, angle) + position,
      glm::rotate(glm::vec2{half_width, -half_height}, angle) + position,
      glm::rotate(glm::vec2{half_width, half_height}, angle) + position,
  };
}
} // namespace GameEngine
