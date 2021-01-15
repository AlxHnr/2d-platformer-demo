/** @file
 * Implements an interactive character which can run, jump and fall.
 */

#include "Physics/DynamicObject.hpp"
#include "Geometry.hpp"
#include <glm/gtx/projection.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace GameEngine::Physics {
DynamicObject::DynamicObject(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {}

bool DynamicObject::isTouchingGround() const { return ground_normal.has_value(); }

std::optional<glm::vec2> DynamicObject::isTouchingWall() const {
  return direction_to_colliding_wall;
}

glm::vec2 DynamicObject::getUpDirection() const { return ground_normal.value_or(glm::vec2{0, 1}); }

glm::vec2 DynamicObject::getRightDirection() const {
  if (!ground_normal.has_value()) {
    /* Fall back to X axis. */
    return {1, 0};
  }
  return {ground_normal->y, -ground_normal->x};
}

void DynamicObject::update() {
  /* Align velocity parallel to ground when moving towards ground. */
  if (ground_normal.has_value() &&
      glm::angle(*ground_normal, glm::normalize(velocity)) > glm::half_pi<float>()) {
    velocity = glm::proj(velocity, getRightDirection());
  }
  if (is_touching_ceiling) {
    velocity = glm::proj(velocity, -getUpDirection());
    velocity.y = glm::min(velocity.y, 0.0f);
  }
  if (ground_normal.has_value()) {
    velocity *= 1 - ground_stickiness;
  }
  if (direction_to_colliding_wall.has_value()) {
    const float wall_gravity = 0.0001; /* Small value to keep objects sticking to walls. */
    const float wall_resistance = 0.5;
    const auto x_direction_towards_wall = direction_to_colliding_wall->x < 0 ? -1 : 1;
    const bool moving_left = velocity.x < 0;
    const bool wall_is_left = direction_to_colliding_wall->x < 0;

    if (wall_is_left != moving_left || glm::abs(velocity.x) < wall_gravity) {
      velocity.x += x_direction_towards_wall * wall_gravity;
    }
    velocity.x *= 1 - wall_resistance;
    velocity *= 1 - wall_stickiness;
  }
  velocity *= 1 - air_friction;
  velocity -= getUpDirection() * gravity;

  ground_normal.reset();
  direction_to_colliding_wall.reset();
  is_touching_ceiling = false;
}

void DynamicObject::addVelocityOffset(const glm::vec2 offset) {
  bounding_polygon.setPosition(bounding_polygon.getPosition() + offset);
}

const ConvexBoundingPolygon &DynamicObject::getBoundingPolygon() const { return bounding_polygon; }

void DynamicObject::handleCollisionWith(Physics::Object &, const glm::vec2 displacement_vector) {
  addVelocityOffset(displacement_vector);

  const auto normalized_displacement_vector = glm::normalize(displacement_vector);
  const auto angle = glm::angle(glm::vec2{0, 1}, normalized_displacement_vector);
  if (angle < glm::radians(55.0f) || angle > glm::radians(90.1f)) {
    const bool is_falling = velocity.y < 0;
    const bool other_object_below_self = displacement_vector.y > 0;

    if (other_object_below_self) {
      ground_normal = normalized_displacement_vector;
    } else if (!is_falling) {
      is_touching_ceiling = true;
    }
  } else {
    const bool is_moving_right = velocity.x > 0;
    const bool other_object_right_of_self = displacement_vector.x < 0;
    if (glm::abs(velocity.x) < glm::epsilon<float>() ||
        is_moving_right == other_object_right_of_self) {
      direction_to_colliding_wall = glm::normalize(-displacement_vector);
    }
  }
}

void DynamicObject::render(SDL_Renderer *renderer, const Camera &camera, float) const {
  if (ground_normal.has_value()) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  } else if (direction_to_colliding_wall.has_value()) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  }
  Geometry::forEachEdge(bounding_polygon.getVertices(),
                        [&](const glm::vec2 world_start, const glm::vec2 world_end) {
                          const auto start = camera.toScreenCoordinate(world_start);
                          const auto end = camera.toScreenCoordinate(world_end);
                          SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
                        });

  const auto position_on_screen = camera.toScreenCoordinate(bounding_polygon.getPosition());
  const auto right_direction_on_screen_end =
      camera.toScreenCoordinate(bounding_polygon.getPosition() + getRightDirection());
  const auto velocity_on_screen_end =
      camera.toScreenCoordinate(bounding_polygon.getPosition() + velocity * 7.5f);

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_RenderDrawLine(renderer, position_on_screen.x, position_on_screen.y,
                     right_direction_on_screen_end.x, right_direction_on_screen_end.y);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(renderer, position_on_screen.x, position_on_screen.y, velocity_on_screen_end.x,
                     velocity_on_screen_end.y);
}

glm::vec2 DynamicObject::getVelocity() const { return velocity; }

void DynamicObject::setVelocity(const glm::vec2 velocity) { this->velocity = velocity; }

float DynamicObject::getGravity() const { return gravity; }

void DynamicObject::setGravity(const float gravity) { this->gravity = glm::max(gravity, 0.0f); }

float DynamicObject::getGroundStickiness() const { return ground_stickiness; }

void DynamicObject::setGroundStickiness(const float ground_stickiness) {
  this->ground_stickiness = glm::clamp(ground_stickiness, 0.0f, 1.0f);
}

float DynamicObject::getWallStickiness() const { return wall_stickiness; }

void DynamicObject::setWallStickiness(const float wall_stickiness) {
  this->wall_stickiness = glm::clamp(wall_stickiness, 0.0f, 1.0f);
}

float DynamicObject::getAirFriction() const { return gravity; }

void DynamicObject::setAirFriction(const float air_friction) {
  this->air_friction = glm::clamp(air_friction, 0.0f, 1.0f);
}
} // namespace GameEngine::Physics
