/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include "Geometry.hpp"
#include <SDL.h>
#include <glm/geometric.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <nonstd/span.hpp>
#include <optional>

namespace {
void renderPolygon(SDL_Renderer *renderer, nonstd::span<const glm::vec2> points) {
  GameEngine::Geometry::forEachEdge(points, [&](const glm::vec2 &start, const glm::vec2 &end) {
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}
} // namespace

namespace GameEngine {
Game::Game() : game_character{{50, 300}, {50, 350}, {100, 350}, {100, 300}} {
  objects.push_back(PolygonObject{{10, 10}, {1270, 10}});    /* Ceiling. */
  objects.push_back(PolygonObject{{10, 10}, {10, 780}});     /* Left wall. */
  objects.push_back(PolygonObject{{10, 780}, {1270, 780}});  /* Right wall. */
  objects.push_back(PolygonObject{{1270, 10}, {1270, 780}}); /* Floor. */

  const glm::vec2 box_half_width = {75, 0};
  const glm::vec2 box_half_height = {0, 75};
  const glm::vec2 box_center{870, 780 - box_half_height.y};
  objects.push_back(PolygonObject{
      box_center - box_half_width + box_half_height,
      box_center - box_half_width - box_half_height,
      box_center + box_half_width - box_half_height,
      box_center + box_half_width + box_half_height,
  });

  objects.push_back(PolygonObject{{450, 780}, {650, 780}, {650, 670}});    /* Ramp. */
  objects.push_back(PolygonObject{{750, 470}, {790, 520}, {620, 470}});    /* Plattform. */
  objects.push_back(PolygonObject{{550, 320}, {590, 370}, {420, 320}});    /* Plattform. */
  objects.push_back(PolygonObject{{1150, 780}, {1270, 780}, {1270, 470}}); /* Steep ramp. */
}

void Game::scheduleJump() { tick_of_jump_request = current_tick; }

void Game::accelerate(const Acceleration direction) { acceleration_direction = direction; }

void Game::integratePhysics() {
  game_character.setPosition(game_character.getPosition() + game_character.getVelocity());

  for (const auto &object : objects) {
    const auto displacement_vector =
        Geometry::checkCollision(game_character.getBoundingPolygon(), object.getBoundingPolygon());
    if (!displacement_vector || glm::length(*displacement_vector) < glm::epsilon<float>()) {
      continue;
    }
    game_character.setPosition(game_character.getPosition() + *displacement_vector);

    if (glm::abs(displacement_vector->x) > glm::abs(displacement_vector->y)) {
      /* Horizontal collision. */
      const bool character_moves_right = game_character.getVelocity().x > 0;
      const bool object_right_of_character = displacement_vector->x < 0;
      if (character_moves_right == object_right_of_character) {
        game_character.setVelocity({0, game_character.getVelocity().y});
        tick_of_last_wall_collision = current_tick;
      }

      wall_jump_to_right = !object_right_of_character;
    } else {
      /* Vertical collision. */
      const bool character_falls = game_character.getVelocity().y > 0;
      const bool object_below_character = displacement_vector->y < 0;

      if (character_falls && object_below_character) {
        game_character.setVelocity({game_character.getVelocity().x, 0.0});
        right_direction =
            glm::normalize(glm::vec2{-displacement_vector->y, displacement_vector->x});
        tick_of_last_floor_collision = current_tick;
      } else if (!character_falls && !object_below_character) {
        game_character.setVelocity({game_character.getVelocity().x, 0.0});
      }
    }
  }

  if (jumpScheduled()) {
    if (collidesWithFloor()) {
      tick_of_jump_request = 0;
      game_character.setVelocity(game_character.getVelocity() + glm::vec2{0, -15});
    } else if (collidesWithWall()) {
      tick_of_jump_request = 0;
      const auto inversion_factor = wall_jump_to_right ? 1 : -1;
      const glm::vec2 next_jump_direction = {
          glm::rotate(glm::vec2{0, -1}, glm::radians(45.0f)).x * inversion_factor, -1};
      game_character.setVelocity(next_jump_direction * 15.0f);
    }
  }

  if (!collidesWithFloor()) {
    right_direction = {1, 0};
  }

  const auto acceleration_vector =
      acceleration_direction == Acceleration::Left ? -right_direction : right_direction;
  const bool accelerating_in_moving_direction =
      glm::dot(game_character.getVelocity(), acceleration_vector) > 0;
  if (acceleration_direction == Acceleration::None) {
    const glm::vec2 friction_factor{0.95, 1};
    game_character.setVelocity(game_character.getVelocity() * friction_factor);
  } else if (!accelerating_in_moving_direction ||
             glm::length(glm::proj(game_character.getVelocity(), acceleration_vector)) < 10.0) {
    game_character.setVelocity(game_character.getVelocity() + acceleration_vector);
  }

  const glm::vec2 gravity{0, 0.5};
  game_character.setVelocity(game_character.getVelocity() + gravity);
  current_tick++;
} // namespace GameEngine

void Game::render(SDL_Renderer *renderer) const {
  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (const auto &object : objects) {
    renderPolygon(renderer, object.getBoundingPolygon());
  }

  if (collidesWithFloor() || collidesWithWall()) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
  }
  renderPolygon(renderer, game_character.getBoundingPolygon());

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  renderPolygon(renderer,
                std::array{game_character.getPosition(),
                           game_character.getPosition() + game_character.getVelocity() * 50.0f});

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  renderPolygon(renderer, std::array{game_character.getPosition(),
                                     game_character.getPosition() + right_direction * 50.0f});
}

bool Game::jumpScheduled() const { return current_tick - tick_of_jump_request < 6; }
bool Game::collidesWithFloor() const { return current_tick - tick_of_last_floor_collision < 6; }
bool Game::collidesWithWall() const { return current_tick - tick_of_last_wall_collision < 6; }
} // namespace GameEngine
