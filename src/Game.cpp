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
  objects.push_back(PolygonObject{{1150, 780}, {1270, 780}, {1270, 470}}); /* Steep ramp. */
}

void Game::integratePhysics() {
  game_character.setPosition(game_character.getPosition() + game_character.getVelocity());

  for (const auto &object : objects) {
    const auto displacement_vector =
        Geometry::checkCollision(game_character.getBoundingPolygon(), object.getBoundingPolygon());
    if (!displacement_vector) {
      continue;
    }

    game_character.setPosition(game_character.getPosition() + *displacement_vector);
    game_character.setVelocity(game_character.getVelocity() * glm::vec2{0, -0.4});
    break;
  }

  const glm::vec2 gravity{0, 0.5};
  game_character.setVelocity(game_character.getVelocity() + gravity);
}

void Game::render(SDL_Renderer *renderer) const {
  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (const auto &object : objects) {
    renderPolygon(renderer, object.getBoundingPolygon());
  }

  SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
  renderPolygon(renderer, game_character.getBoundingPolygon());
}
} // namespace GameEngine
