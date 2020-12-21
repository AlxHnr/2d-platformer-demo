/** @file
 * Implements a playground for polygons.
 */

#include "PolygonDemo.hpp"
#include <SDL.h>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <nonstd/span.hpp>

namespace {
/** Collects all normal vectors from the given polygon.
 *
 * @param points Vertices of the polygon.
 *
 * @return Normal vectors for each side of the polygon.
 */
std::vector<glm::vec2> collectNormals(nonstd::span<const glm::vec2> points) {
  std::vector<glm::vec2> result{};

  const auto *previous_point = &points.back();
  for (const auto &point : points) {
    const auto normal =
        glm::normalize(glm::vec2{point.y - previous_point->y, previous_point->x - point.x});
    result.push_back(normal);
    previous_point = &point;
  }

  return result;
}

void renderPolygon(SDL_Renderer *renderer, nonstd::span<const glm::vec2> points,
                   const bool collision) {
  const auto *previous_point = &points.back();
  for (const auto &point : points) {
    if (collision) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
    } else {
      SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
    }
    SDL_RenderDrawLine(renderer, previous_point->x, previous_point->y, point.x, point.y);

    const auto center = (*previous_point + point) / 2.0f;
    const auto normal =
        glm::normalize(glm::vec2{point.y - previous_point->y, previous_point->x - point.x}) * 35.0f;
    SDL_SetRenderDrawColor(renderer, 255, 180, 180, 255);
    SDL_RenderDrawLine(renderer, center.x, center.y, center.x + normal.x, center.y + normal.y);

    previous_point = &point;
  }
}

struct ProjectedEdges {
  float min;
  float max;
};

/** Projects all vertices in the given polygon onto the given axis and returns the minmax values.
 *
 * @param points Represents the polygon.
 * @param axis All points in the given polygon will be projected onto this.
 *
 * @return Min and max values of all vertices projected onto the specified axis.
 */
ProjectedEdges projectPolygonToNormal(nonstd::span<const glm::vec2> points, const glm::vec2 &axis) {
  std::vector<float> dot_products;
  dot_products.reserve(points.size());
  std::transform(points.begin(), points.end(), std::back_inserter(dot_products),
                 [&](const glm::vec2 &point) { return glm::dot(point, axis); });
  const auto [min, max] = std::minmax_element(dot_products.begin(), dot_products.end());
  return {*min, *max};
}

bool collidePolygons(SDL_Renderer *renderer, nonstd::span<const glm::vec2> a,
                     nonstd::span<const glm::vec2> b, const int offset) {
  const glm::vec2 screen_center{640, 400};
  int iteration = 1;

  for (const auto &normal : collectNormals(a)) {
    const auto green_vector = normal * 1000.0f;
    SDL_SetRenderDrawColor(renderer, 180, 255, 180, 255);
    SDL_RenderDrawLine(renderer, screen_center.x, screen_center.y, screen_center.x + green_vector.x,
                       screen_center.y + green_vector.y);
    SDL_RenderDrawLine(renderer, screen_center.x, screen_center.y,
                       screen_center.x + glm::length(green_vector), screen_center.y);

    const auto a_edges = projectPolygonToNormal(a, normal);
    const auto b_edges = projectPolygonToNormal(b, normal);

    if (a_edges.min > b_edges.max || a_edges.max < b_edges.min) {
      return false;
    }

    const auto a_distance = a_edges.max - a_edges.min;
    const auto b_distance = b_edges.max - b_edges.min;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, screen_center.x, screen_center.y - iteration * 2 + offset,
                       screen_center.x + a_distance, screen_center.y - iteration * 2 + offset);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderDrawLine(renderer, screen_center.x, screen_center.y + iteration * 2 + offset,
                       screen_center.x + b_distance, screen_center.y + iteration * 2 + offset);

    iteration++;
  }

  return true;
}
} // namespace

namespace GameEngine {
PolygonDemo::PolygonDemo()
    : rectangle{{{170, 170}, {550, 290}, {450, 380}, {270, 300}}}, triangle{{{575, 400},
                                                                             {792, 515},
                                                                             {870, 670}}} {}

void PolygonDemo::handleFrame(SDL_Renderer *renderer, const std::chrono::microseconds) {
  const auto move_factor = (SDL_GetTicks() % 20000) / 100.0f;
  const auto rotation_speed = 0.05f;

  auto moved_rectangle = rectangle;
  for (auto &point : moved_rectangle) {
    const auto matrix =
        glm::translate(glm::vec3{move_factor, move_factor, 0.0f}) *
        glm::translate(glm::vec3{350.0f, 275.0f, 0.0f}) *
        glm::rotate(glm::mat4{1}, rotation_speed * move_factor, glm::vec3{0.0f, 0.0f, 1.0f}) *
        glm::translate(glm::vec3{-350.0f, -275.0f, 0.0f});
    point = glm::vec2{matrix * glm::vec4{point, 0.0f, 1.0f}};
  }

  auto moved_triangle = triangle;
  for (auto &point : moved_triangle) {
    const auto matrix =
        glm::translate(-glm::vec3{move_factor / 1.25, move_factor, 0.0f}) *
        glm::translate(glm::vec3{750.0f, 470.0f, 0.0f}) *
        glm::rotate(glm::mat4{1}, -rotation_speed * move_factor, glm::vec3{0.0f, 0.0f, 1.0f}) *
        glm::translate(glm::vec3{-750.0f, -470.0f, 0.0f});
    point = glm::vec2{matrix * glm::vec4{point, 0.0f, 1.0f}};
  }

  const auto ab = collidePolygons(renderer, moved_rectangle, moved_triangle, 0);
  const auto ba = collidePolygons(renderer, moved_triangle, moved_rectangle, 50);
  renderPolygon(renderer, moved_rectangle, ab && ba);
  renderPolygon(renderer, moved_triangle, ab && ba);
}
} // namespace GameEngine
