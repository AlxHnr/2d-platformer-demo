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
  GameEngine::Geometry::forEachEdge(points,
                                    [&](size_t, const glm::vec2 &start, const glm::vec2 &end) {
                                      SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
                                    });
}
} // namespace

namespace GameEngine {
void PolygonDemo::handleFrame(SDL_Renderer *renderer, const std::chrono::microseconds) {
  const auto move_factor = (SDL_GetTicks() % 20000) / 100.0f;
  const auto rotation_speed = 0.05f;

  std::array rectangle = {
      glm::vec2{170, 170},
      glm::vec2{270, 300},
      glm::vec2{450, 380},
      glm::vec2{550, 290},
  };
  std::array triangle{
      glm::vec2{870, 670},
      glm::vec2{792, 515},
      glm::vec2{575, 400},
  };

  for (auto &point : rectangle) {
    const auto matrix =
        glm::translate(glm::vec3{move_factor, move_factor, 0.0f}) *
        glm::translate(glm::vec3{350.0f, 275.0f, 0.0f}) *
        glm::rotate(glm::mat4{1}, rotation_speed * move_factor, glm::vec3{0.0f, 0.0f, 1.0f}) *
        glm::translate(glm::vec3{-350.0f, -275.0f, 0.0f});
    point = glm::vec2{matrix * glm::vec4{point, 0.0f, 1.0f}};
  }

  for (auto &point : triangle) {
    const auto matrix =
        glm::translate(-glm::vec3{move_factor / 1.25, move_factor, 0.0f}) *
        glm::translate(glm::vec3{750.0f, 470.0f, 0.0f}) *
        glm::rotate(glm::mat4{1}, -rotation_speed * move_factor, glm::vec3{0.0f, 0.0f, 1.0f}) *
        glm::translate(glm::vec3{-750.0f, -470.0f, 0.0f});
    point = glm::vec2{matrix * glm::vec4{point, 0.0f, 1.0f}};
  }

  std::array rectangle_polygon{rectangle[0], rectangle[1], rectangle[2], rectangle[3]};
  std::array triangle_polygon{triangle[0], triangle[1], triangle[2]};

  const auto displacement_vector =
      Geometry::checkPolygonCollision(triangle_polygon, rectangle_polygon);
  if (displacement_vector.has_value()) {
    for (auto &point : triangle) {
      point += *displacement_vector / 2.0f;
    }
    for (auto &point : rectangle) {
      point -= *displacement_vector / 2.0f;
    }
  }

  renderPolygon(renderer, rectangle, displacement_vector.has_value());
  renderPolygon(renderer, triangle, displacement_vector.has_value());
}
} // namespace GameEngine
