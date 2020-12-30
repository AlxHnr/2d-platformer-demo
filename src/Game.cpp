/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include "Geometry.hpp"
#include <SDL.h>
#include <glm/geometric.hpp>
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

void Game::scheduleJump() { jump_scheduled = true; }

void Game::accelerate(const Acceleration direction) { acceleration_direction = direction; }

void Game::integratePhysics() {
  game_character.setPosition(game_character.getPosition() + game_character.getVelocity());

  frames_since_floor_was_touched++;
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
      if ((character_moves_right && object_right_of_character) ||
          (!character_moves_right && !object_right_of_character)) {
        game_character.setVelocity({0, game_character.getVelocity().y});
      }
    } else {
      /* Vertical collision. */
      const bool character_falls = game_character.getVelocity().y > 0;
      const bool object_below_character = displacement_vector->y < 0;

      if (character_falls && object_below_character) {
        game_character.setVelocity({game_character.getVelocity().x, 0.0});
        frames_since_floor_was_touched = 0;
        right_direction =
            glm::normalize(glm::vec2{-displacement_vector->y, displacement_vector->x});
      } else if (!character_falls && !object_below_character) {
        game_character.setVelocity({game_character.getVelocity().x, 0.0});
      }
    }
  }

  if (isTouchingFloor()) {
    if (jump_scheduled) {
      const glm::vec2 jump_strength{0, -15.0};
      game_character.setVelocity(game_character.getVelocity() + jump_strength);
      frames_since_floor_was_touched += 10;
    }
  } else {
    right_direction = {1, 0};
  }

  /* Air/ground friction. */
  game_character.setVelocity(game_character.getVelocity() * glm::vec2{0.95, 1});

  switch (acceleration_direction) {
  case Acceleration::None:
    break;
  case Acceleration::Left:
    game_character.setVelocity(game_character.getVelocity() - right_direction * 0.5f);
    break;
  case Acceleration::Right:
    game_character.setVelocity(game_character.getVelocity() + right_direction * 0.5f);
    break;
  }

  const glm::vec2 gravity{0, 0.5};
  game_character.setVelocity(game_character.getVelocity() + gravity);

  jump_scheduled = false;
}

void Game::render(SDL_Renderer *renderer) const {
  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (const auto &object : objects) {
    renderPolygon(renderer, object.getBoundingPolygon());
  }

  if (isTouchingFloor()) {
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

bool Game::isTouchingFloor() const { return frames_since_floor_was_touched < 10; }
} // namespace GameEngine
