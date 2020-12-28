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
  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  GameEngine::Geometry::forEachEdge(points, [&](const glm::vec2 &start, const glm::vec2 &end) {
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}
} // namespace

namespace GameEngine {
Game::Game() {
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

void Game::handleFrame(SDL_Renderer *renderer, const std::chrono::microseconds) {
  for (const auto &object : objects) {
    renderPolygon(renderer, object.getBoundingPolygon());
  }
}
} // namespace GameEngine
