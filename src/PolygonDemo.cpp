/** @file
 * Implements a playground for polygons.
 */

#include "PolygonDemo.hpp"
#include "Geometry.hpp"
#include <SDL.h>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <nonstd/span.hpp>
#include <optional>

namespace {
void renderPolygon(SDL_Renderer *renderer, nonstd::span<const glm::vec2> points,
                   const bool collision) {
  if (collision) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  }
  GameEngine::Geometry::forEachEdge(points, [&](const glm::vec2 &start, const glm::vec2 &end) {
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}
} // namespace

namespace GameEngine {
PolygonDemo::PolygonDemo()
    : triangle{{870, 670}, {792, 515}, {575, 400}}, rectangle{
                                                        {170, 170},
                                                        {270, 300},
                                                        {450, 380},
                                                        {550, 290},
                                                    } {}

void PolygonDemo::handleFrame(SDL_Renderer *renderer, const std::chrono::microseconds) {
  const auto move_factor = (SDL_GetTicks() % 20000) / 100.0f;
  const auto rotation_speed = 0.05f;

  rectangle.rotate(rotation_speed);
  triangle.rotate(-rotation_speed);
  triangle.setPosition(triangle.getPosition() - move_factor / 10);

  const auto displacement_vector =
      Geometry::checkCollision(triangle.getBoundingPolygon(), rectangle.getBoundingPolygon());
  if (displacement_vector.has_value()) {
    triangle.setPosition(triangle.getPosition() + *displacement_vector);
  }

  renderPolygon(renderer, rectangle.getBoundingPolygon(), displacement_vector.has_value());
  renderPolygon(renderer, triangle.getBoundingPolygon(), displacement_vector.has_value());
}
} // namespace GameEngine
