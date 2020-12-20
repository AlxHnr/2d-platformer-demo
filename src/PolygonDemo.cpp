/** @file
 * Implements a playground for polygons.
 */

#include "PolygonDemo.hpp"
#include <SDL.h>
#include <cmath>
#include <nonstd/span.hpp>

namespace {
glm::vec2 normalize(const glm::vec2 &point) {
  const auto length = sqrt(point.x * point.x + point.y * point.y) / 35;
  return {static_cast<int>(point.x / length), static_cast<int>(point.y / length)};
}

void renderPolygon(SDL_Renderer *renderer, nonstd::span<glm::vec2> points) {
  const auto *previous_point = &points.back();
  for (const auto &point : points) {
    SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
    SDL_RenderDrawLine(renderer, previous_point->x, previous_point->y, point.x, point.y);

    const glm::vec2 center{(previous_point->x + point.x) / 2, (previous_point->y + point.y) / 2};
    const auto normal = normalize({point.y - previous_point->y, previous_point->x - point.x});
    SDL_SetRenderDrawColor(renderer, 255, 180, 180, 255);
    SDL_RenderDrawLine(renderer, center.x, center.y, center.x + normal.x, center.y + normal.y);

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
