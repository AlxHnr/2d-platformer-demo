/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <nonstd/span.hpp>

namespace {
void renderPolygon(SDL_Renderer *renderer, const GameEngine::ConvexBoundingPolygon &polygon) {
  polygon.forEachEdge([&](const glm::vec2 &start, const glm::vec2 &end) {
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}

GameEngine::GameCharacter makeBox(const glm::vec2 &center, const float width, const float height) {
  const glm::vec2 box_half_width = {width / 2, 0};
  const glm::vec2 box_half_height = {0, height / 2};
  const glm::vec2 box_center{center.x - box_half_width.x, center.y - box_half_height.y};
  return {
      box_center - box_half_width + box_half_height,
      box_center - box_half_width - box_half_height,
      box_center + box_half_width - box_half_height,
      box_center + box_half_width + box_half_height,
  };
}
} // namespace

namespace GameEngine {
Game::Game() {
  objects.push_back(makeBox({65, 305}, 40, 40));

  objects.push_back({{10, 10}, {1270, 10}});    /* Ceiling. */
  objects.push_back({{10, 10}, {10, 780}});     /* Left wall. */
  objects.push_back({{1270, 10}, {1270, 780}}); /* Right wall. */
  objects.push_back({{10, 780}, {1270, 780}});  /* Floor. */

  objects.push_back(makeBox({945, 780}, 150, 150));
  for (size_t index = 0; index < 24; ++index) {
    const float width = 15;
    objects.push_back({{745 + width * index, 320}, {745 + width * index, 325}});
  }

  objects.push_back({{450, 780}, {650, 780}, {795, 630}});    /* Ramp. */
  objects.push_back({{10, 600}, {10, 780}, {340, 780}});      /* Ramp. */
  objects.push_back({{750, 470}, {790, 520}, {620, 470}});    /* Plattform. */
  objects.push_back({{550, 320}, {590, 370}, {420, 320}});    /* Plattform. */
  objects.push_back({{1150, 780}, {1270, 780}, {1270, 470}}); /* Steep ramp. */
}

GameCharacter &Game::getGameCharacter() { return objects.front(); }

void Game::integratePhysics() {
  auto &object = objects.front();

  const auto acceleration_vector =
      object.acceleration_direction == GameCharacter::VerticalAcceleration::Left
          ? -object.right_direction
          : object.right_direction;
  const bool accelerating_in_moving_direction = glm::dot(object.velocity, acceleration_vector) > 0;
  if (object.acceleration_direction == GameCharacter::VerticalAcceleration::None) {
    const glm::vec2 friction_factor{0.95, 1};
    object.velocity *= friction_factor;
  } else if (!accelerating_in_moving_direction ||
             glm::length(glm::proj(object.velocity, acceleration_vector)) < 10.0) {
    object.velocity += acceleration_vector;
  }

  /* Apply gravity perpendicular to current slope. */
  glm::vec2 down{-object.right_direction.y, object.right_direction.x};
  if (object.state == GameCharacter::State::TouchingGround) {
    object.bounding_polygon.setPosition(object.bounding_polygon.getPosition() + down);
  } else if (object.state == GameCharacter::State::TouchingWall) {
    object.velocity.x = object.wall_jump_to_right ? -0.5 : 0.5;
    object.velocity += down * 0.5f;
  } else {
    object.velocity += down * 0.5f;
  }

  if (object.jumpScheduled()) {
    if (object.state == GameCharacter::State::TouchingGround) {
      object.tick_of_jump_request = 0;
      object.velocity.y -= 15;
    } else if (object.state == GameCharacter::State::TouchingWall) {
      object.tick_of_jump_request = 0;
      const auto inversion_factor = object.wall_jump_to_right ? 1 : -1;
      const glm::vec2 next_jump_direction = {
          glm::rotate(glm::vec2{0, -1}, glm::radians(45.0f)).x * inversion_factor, -1};
      object.velocity = next_jump_direction * 15.0f;
    }
  }

  object.bounding_polygon.setPosition(object.bounding_polygon.getPosition() + object.velocity);

  bool touching_ground = false;
  bool touching_wall = false;
  object.right_direction = {1, 0};
  for (size_t i = 1; i < objects.size(); ++i) {
    auto &other_object = objects[i];

    const auto displacement_vector =
        object.bounding_polygon.collidesWith(other_object.bounding_polygon);
    if (!displacement_vector) {
      continue;
    }
    object.bounding_polygon.setPosition(object.bounding_polygon.getPosition() +
                                        *displacement_vector);

    if (glm::abs(displacement_vector->x) < glm::abs(displacement_vector->y)) {
      /* Vertical collision. */
      const bool character_falls = object.velocity.y > 0;
      const bool object_below_character = displacement_vector->y < 0;

      if (object_below_character) {
        object.right_direction =
            glm::normalize(glm::vec2{-displacement_vector->y, displacement_vector->x});
        object.velocity = glm::proj(object.velocity, object.right_direction);
        touching_ground = true;
      } else if (!character_falls && !object_below_character) {
        object.velocity.y = 0;
      }
    } else {
      /* Horizontal collision. */
      const bool character_moves_right = object.velocity.x > 0;
      const bool object_right_of_character = displacement_vector->x < 0;
      if (character_moves_right == object_right_of_character) {
        touching_wall = true;
      }
      object.wall_jump_to_right = !object_right_of_character;
    }
  }

  if (touching_ground) {
    object.state = GameCharacter::State::TouchingGround;
  } else if (touching_wall) {
    object.state = GameCharacter::State::TouchingWall;
  } else {
    object.state = GameCharacter::State::Falling;
  }

  object.current_tick++;
}

void Game::render(SDL_Renderer *renderer) const {
  const auto &game_character = objects.front();

  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (size_t index = 1; index < objects.size(); ++index) {
    renderPolygon(renderer, objects[index].bounding_polygon);
  }

  if (game_character.state == GameCharacter::State::TouchingGround) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  } else if (game_character.state == GameCharacter::State::TouchingWall) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
  }
  renderPolygon(renderer, game_character.bounding_polygon);

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_RenderDrawLine(
      renderer, game_character.bounding_polygon.getPosition().x,
      game_character.bounding_polygon.getPosition().y,
      (game_character.bounding_polygon.getPosition() + game_character.right_direction * 50.0f).x,
      (game_character.bounding_polygon.getPosition() + game_character.right_direction * 50.0f).y);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(
      renderer, game_character.bounding_polygon.getPosition().x,
      game_character.bounding_polygon.getPosition().y,
      (game_character.bounding_polygon.getPosition() + game_character.velocity * 50.0f).x,
      (game_character.bounding_polygon.getPosition() + game_character.velocity * 50.0f).y);
}
} // namespace GameEngine
