/** @file
 * Implements a playground for polygons.
 */

#include "PolygonDemo.hpp"
#include "ConvexBoundingPolygon.hpp"
#include "Geometry.hpp"
#include <SDL.h>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <nonstd/span.hpp>
#include <optional>

namespace {
/** Collects all normal vectors from the given polygon.
 *
 * @param points Vertices of the polygon.
 *
 * @return Normal vectors for each side of the polygon.
 */
std::vector<glm::vec2> collectNormals(nonstd::span<const glm::vec2> points) {
  std::vector<glm::vec2> result{};

  GameEngine::Geometry::forEachEdge(
      points, [&](size_t, const glm::vec2 &start, const glm::vec2 &end) {
        const auto normal = GameEngine::Geometry::computeNormalOfEdge(start, end);
        result.push_back(normal);
      });

  return result;
}

void renderPolygon(SDL_Renderer *renderer, nonstd::span<const glm::vec2> points,
                   const bool collision) {
  GameEngine::Geometry::forEachEdge(
      points, [&](size_t, const glm::vec2 &start, const glm::vec2 &end) {
        if (collision) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
        } else {
          SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
        }
        SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);

        const auto center = (start + end) / 2.0f;
        const auto normal = GameEngine::Geometry::computeNormalOfEdge(start, end) * 35.0f;
        SDL_SetRenderDrawColor(renderer, 255, 180, 180, 255);
        SDL_RenderDrawLine(renderer, center.x, center.y, center.x + normal.x, center.y + normal.y);
      });
}

struct ProjectedEdges {
  float min;
  float max;
  float distance;
};

/** Projects all vertices in the given polygon onto the given axis and returns the minmax values.
 *
 * @param points Represents the polygon.
 * @param axis All points in the given polygon will be projected onto this.
 *
 * @return Min and max values of all vertices projected onto the specified axis.
 */
ProjectedEdges projectPolygonToNormal(nonstd::span<const glm::vec2> points, const glm::vec2 &axis) {
  const auto values = GameEngine::Geometry::projectVerticesOntoAxisMinMax(points, axis);
  return {values.min, values.max, values.max - values.min};
}

std::optional<glm::vec2> collidePolygons(SDL_Renderer *renderer, nonstd::span<const glm::vec2> a,
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
      return std::nullopt;
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, screen_center.x, screen_center.y - iteration * 2 + offset,
                       screen_center.x + a_edges.distance,
                       screen_center.y - iteration * 2 + offset);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderDrawLine(renderer, screen_center.x, screen_center.y + iteration * 2 + offset,
                       screen_center.x + b_edges.distance,
                       screen_center.y + iteration * 2 + offset);

    iteration++;
  }

  return glm::vec2{};
}
} // namespace

namespace GameEngine {
PolygonDemo::PolygonDemo()
    : rectangle{{
          {170, 170},
          {270, 300},
          {450, 380},
          {550, 290},
      }},
      triangle{{
          {870, 670},
          {792, 515},
          {575, 400},
      }} {
  ConvexBoundingPolygon empty{};
  ConvexBoundingPolygon one{glm::vec2{792, 515}};
  ConvexBoundingPolygon line{
      glm::vec2{792, 515},
      glm::vec2{870, 670},
  };
  ConvexBoundingPolygon<3> test1{
      glm::vec2{575, 400},
      glm::vec2{792, 515},
      glm::vec2{870, 670},
  };
  ConvexBoundingPolygon<3> test2{
      triangle.at(0),
      triangle.at(1),
      triangle.at(2),
  };
  ConvexBoundingPolygon test3{
      glm::vec2{170, 170},
      glm::vec2{270, 300},
      glm::vec2{450, 380},
      glm::vec2{550, 290},
  };
}

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

  collidePolygons(renderer, moved_rectangle, moved_triangle, 0);
  collidePolygons(renderer, moved_triangle, moved_rectangle, 50);

  std::array<Geometry::ProjectedVertices, 3> projected_triangle;
  std::array<Geometry::ProjectedVertices, 4> projected_rectangle;
  Geometry::forEachEdge(
      moved_triangle, [&](const size_t index, const glm::vec2 &start, const glm::vec2 &end) {
        const auto axis = Geometry::computeNormalOfEdge(start, end);
        projected_triangle[index] = Geometry::projectVerticesOntoAxisMinMax(moved_triangle, axis);
      });
  Geometry::forEachEdge(
      moved_rectangle, [&](const size_t index, const glm::vec2 &start, const glm::vec2 &end) {
        const auto axis = Geometry::computeNormalOfEdge(start, end);
        projected_rectangle[index] = Geometry::projectVerticesOntoAxisMinMax(moved_rectangle, axis);
        ;
      });

  const auto displacement_vector = Geometry::checkPolygonCollision(
      moved_rectangle, projected_rectangle, moved_triangle, projected_triangle);
  if (displacement_vector.has_value()) {
    for (auto &point : moved_rectangle) {
      point += *displacement_vector;
    }
  }

  renderPolygon(renderer, moved_rectangle, displacement_vector.has_value());
  renderPolygon(renderer, moved_triangle, displacement_vector.has_value());
}
} // namespace GameEngine
