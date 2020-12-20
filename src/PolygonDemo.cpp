/** @file
 * Implements a playground for polygons.
 */

#include "PolygonDemo.hpp"
#include <SDL.h>
#include <nonstd/span.hpp>

namespace {
void renderPolygon(SDL_Renderer *renderer, nonstd::span<GameEngine::PolygonDemo::Point> points) {
  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);

  const auto *previous_point = &points.back();
  for (const auto &point : points) {
    SDL_RenderDrawLine(renderer, previous_point->x, previous_point->y, point.x, point.y);
    previous_point = &point;
  }
}
} // namespace

namespace GameEngine {
PolygonDemo::PolygonDemo()
    : rectangle{{{170, 170}, {550, 290}, {450, 380}, {270, 300}}}, triangle{{{575, 400},
                                                                             {792, 515},
                                                                             {870, 670}}} {}

void PolygonDemo::handleFrame(SDL_Renderer *renderer, const std::chrono::microseconds) {
  const auto move_factor = SDL_GetTicks() / 20 % 200;

  auto moved_rectangle = rectangle;
  for (auto &points : moved_rectangle) {
    points.x += 1 * move_factor;
    points.y += 1 * move_factor;
  }

  auto moved_triangle = triangle;
  for (auto &points : moved_triangle) {
    points.x -= 1 * move_factor;
    points.y -= 1 * move_factor;
  }

  renderPolygon(renderer, moved_rectangle);
  renderPolygon(renderer, moved_triangle);
}
} // namespace GameEngine
