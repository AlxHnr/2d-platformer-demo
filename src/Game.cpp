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

void Game::accelerateCharacter(Direction direction) {
  const auto velocity = game_character.getVelocity();
  const float acceleration = 0.2;
  const float speed_limit = 5.5;

  if (direction == Direction::Left && velocity.x > -speed_limit) {
    const float new_x_velocity = glm::max(velocity.x - acceleration, -speed_limit);
    game_character.setVelocity({new_x_velocity, velocity.y});
  } else if (direction == Direction::Right && velocity.x < speed_limit) {
    const float new_x_velocity = glm::min(velocity.x + acceleration, speed_limit);
    game_character.setVelocity({new_x_velocity, velocity.y});
  }
}

void Game::integratePhysics() {
  game_character.setPosition(game_character.getPosition() + game_character.getVelocity());

  is_touching_floor = false;
  for (const auto &object : objects) {
    const auto displacement_vector =
        Geometry::checkCollision(game_character.getBoundingPolygon(), object.getBoundingPolygon());
    if (!displacement_vector) {
      continue;
    }
    is_touching_floor = true;

    game_character.setPosition(game_character.getPosition() + *displacement_vector);

    if (!glm::epsilonEqual(glm::length(*displacement_vector), glm::zero<float>(),
                           glm::epsilon<float>())) {
      last_displacement_vector = *displacement_vector;
    }
  }

  if (is_touching_floor) {
    const auto vertical_velocity = glm::min(game_character.getVelocity().y, 0.0f);
    game_character.setVelocity({game_character.getVelocity().x * 0.95, vertical_velocity});

    if (jump_scheduled) {
      const glm::vec2 jump_strength{0, -10.0};
      game_character.setVelocity(game_character.getVelocity() + jump_strength);
      is_touching_floor = false;
    }
  } else {
    const glm::vec2 gravity{0, 0.5};
    game_character.setVelocity(game_character.getVelocity() + gravity);
  }
  jump_scheduled = false;
}

void Game::render(SDL_Renderer *renderer) const {
  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (const auto &object : objects) {
    renderPolygon(renderer, object.getBoundingPolygon());
  }

  if (is_touching_floor) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
  }
  renderPolygon(renderer, game_character.getBoundingPolygon());

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  renderPolygon(renderer,
                std::array{game_character.getPosition(),
                           game_character.getPosition() + last_displacement_vector * 50.0f});
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  renderPolygon(renderer,
                std::array{game_character.getPosition(),
                           game_character.getPosition() + game_character.getVelocity() * 50.0f});

  const auto slope_normal =
      glm::normalize(glm::vec2{-last_displacement_vector.y, last_displacement_vector.x});
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  renderPolygon(renderer, std::array{game_character.getPosition(),
                                     game_character.getPosition() + slope_normal * 50.0f});
}
} // namespace GameEngine
