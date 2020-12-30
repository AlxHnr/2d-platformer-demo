/** @file
 * Implements an interactive polygon compatible with the physics engine.
 */

#include "PolygonObject.hpp"
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GameEngine {
PolygonObject::PolygonObject(std::initializer_list<glm::vec2> vertices) : vertices{vertices} {
  position = vertices.size() == 0 ? glm::vec2{0, 0} : Geometry::computeCenter(vertices);
  std::transform(vertices.begin(), vertices.end(), std::back_inserter(vertices_relative_to_center),
                 [this](const glm::vec2 &vertex) { return vertex - position; });
}

void PolygonObject::enablePhysics() { physics_enabled = true; }

void PolygonObject::rotate(const float angle) {
  orientation = glm::mod(orientation + angle, glm::pi<float>() * 2);
  recomputeBoundingBox();
}

void PolygonObject::jump() { tick_of_jump_request = current_tick; }

void PolygonObject::accelerate(const PolygonObject::Acceleration direction) {
  acceleration_direction = direction;
}

const glm::vec2 &PolygonObject::getPosition() const { return position; }

void PolygonObject::setPosition(const glm::vec2 &new_position) {
  position = new_position;
  recomputeBoundingBox();
}

const glm::vec2 &PolygonObject::getVelocity() const { return velocity; }

void PolygonObject::setVelocity(const glm::vec2 &new_velocity) { velocity = new_velocity; }

Geometry::ConvexPolygonView PolygonObject::getBoundingPolygon() const { return vertices; }

void PolygonObject::update() {
  if (!physics_enabled) {
    return;
  }

  if (jumpScheduled()) {
    if (collidesWithFloor()) {
      tick_of_jump_request = 0;
      velocity.y -= 15;
    } else if (collidesWithWall()) {
      tick_of_jump_request = 0;
      const auto inversion_factor = wall_jump_to_right ? 1 : -1;
      const glm::vec2 next_jump_direction = {
          glm::rotate(glm::vec2{0, -1}, glm::radians(45.0f)).x * inversion_factor, -1};
      velocity = next_jump_direction * 15.0f;
    }
  }

  if (!collidesWithFloor()) {
    right_direction = {1, 0};
  }

  const auto acceleration_vector =
      acceleration_direction == Acceleration::Left ? -right_direction : right_direction;
  const bool accelerating_in_moving_direction = glm::dot(velocity, acceleration_vector) > 0;
  if (acceleration_direction == Acceleration::None) {
    const glm::vec2 friction_factor{0.95, 1};
    velocity *= friction_factor;
  } else if (!accelerating_in_moving_direction ||
             glm::length(glm::proj(velocity, acceleration_vector)) < 10.0) {
    velocity += acceleration_vector;
  }

  const glm::vec2 gravity{0, 0.5};
  velocity += gravity;
  setPosition(position + velocity);
  recomputeBoundingBox();
  current_tick++;
}

void PolygonObject::handleCollision(PhysicsObject &other_physics_object,
                                    const glm::vec2 &displacement_vector) {
  if (!physics_enabled) {
    return;
  }

  const auto *other = dynamic_cast<PolygonObject *>(&other_physics_object);
  if (other == nullptr) {
    return;
  }
  setPosition(position + displacement_vector);

  if (glm::abs(displacement_vector.x) < glm::abs(displacement_vector.y)) {
    /* Vertical collision. */
    const bool character_falls = velocity.y > 0;
    const bool object_below_character = displacement_vector.y < 0;

    if (character_falls && object_below_character) {
      velocity.y = 0;
      right_direction = glm::normalize(glm::vec2{-displacement_vector.y, displacement_vector.x});
      tick_of_last_floor_collision = current_tick;
    } else if (!character_falls && !object_below_character) {
      velocity.y = 0;
    }
  } else {
    /* Horizontal collision. */
    const bool character_moves_right = velocity.x > 0;
    const bool object_right_of_character = displacement_vector.x < 0;
    if (character_moves_right == object_right_of_character) {
      velocity.x = 0;
      tick_of_last_wall_collision = current_tick;
    }
    wall_jump_to_right = !object_right_of_character;
  }
}

void PolygonObject::recomputeBoundingBox() {
  std::transform(
      vertices_relative_to_center.cbegin(), vertices_relative_to_center.cend(), vertices.begin(),
      [this](const glm::vec2 &vertex) { return glm::rotate(vertex, orientation) + position; });
}

bool PolygonObject::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }
bool PolygonObject::collidesWithFloor() const {
  return current_tick - tick_of_last_floor_collision < 6;
}
bool PolygonObject::collidesWithWall() const {
  return current_tick - tick_of_last_wall_collision < 6;
}
} // namespace GameEngine
