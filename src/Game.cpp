/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include "Geometry.hpp"
#include <nonstd/span.hpp>

namespace {
void renderPolygon(SDL_Renderer *renderer, nonstd::span<const glm::vec2> points) {
  GameEngine::Geometry::forEachEdge(points, [&](const glm::vec2 &start, const glm::vec2 &end) {
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}
} // namespace

namespace GameEngine {
Game::Game() {
  objects.push_back(PolygonObject{{50, 300}, {50, 350}, {100, 350}, {100, 300}});
  objects.front().enablePhysics();

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

PolygonObject &Game::getGameCharacter() { return objects.front(); }

void Game::integratePhysics() {
  for (auto &object : objects) {
    object.update();
  }

  for (size_t i = 0; i < objects.size(); ++i) {
    for (size_t j = i + 1; j < objects.size(); ++j) {
      auto &object1 = objects[i];
      auto &object2 = objects[j];

      const auto displacement_vector =
          Geometry::checkCollision(object1.getBoundingPolygon(), object2.getBoundingPolygon());
      if (!displacement_vector) {
        continue;
      }
      object1.handleCollision(object2, *displacement_vector);
    }
  }
}

void Game::render(SDL_Renderer *renderer) const {
  const auto &game_character = objects.front();

  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (size_t index = 1; index < objects.size(); ++index) {
    renderPolygon(renderer, objects[index].getBoundingPolygon());
  }

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  renderPolygon(renderer, game_character.getBoundingPolygon());

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  renderPolygon(renderer,
                std::array{game_character.getPosition(),
                           game_character.getPosition() + game_character.getVelocity() * 50.0f});
}
} // namespace GameEngine
