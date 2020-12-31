/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <nonstd/span.hpp>

namespace {
void renderPolygon(SDL_Renderer *renderer, const GameEngine::ConvexBoundingPolygon &polygon) {
  polygon.forEachEdge([&](const glm::vec2 &start, const glm::vec2 &end) {
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}

GameEngine::GameCharacter makeBox(const glm::vec2 &center, const float width, const float height) {
  const glm::vec2 box_half_width = {width / 2, 0};
  const glm::vec2 box_half_height = {0, height / 2};
  const glm::vec2 box_center{center.x - box_half_width.x, center.y - box_half_height.y};
  return {
      box_center - box_half_width + box_half_height,
      box_center - box_half_width - box_half_height,
      box_center + box_half_width - box_half_height,
      box_center + box_half_width + box_half_height,
  };
}
} // namespace

namespace GameEngine {
Game::Game() {
  objects.push_back(makeBox({65, 305}, 40, 40));

  objects.push_back({{10, 10}, {1270, 10}});    /* Ceiling. */
  objects.push_back({{10, 10}, {10, 780}});     /* Left wall. */
  objects.push_back({{1270, 10}, {1270, 780}}); /* Right wall. */
  objects.push_back({{10, 780}, {1270, 780}});  /* Floor. */

  objects.push_back(makeBox({945, 780}, 150, 150));
  for (size_t index = 0; index < 24; ++index) {
    const float width = 15;
    objects.push_back({{745 + width * index, 320}, {745 + width * index, 325}});
  }

  objects.push_back({{450, 780}, {650, 780}, {795, 630}});    /* Ramp. */
  objects.push_back({{10, 600}, {10, 780}, {340, 780}});      /* Ramp. */
  objects.push_back({{750, 470}, {790, 520}, {620, 470}});    /* Plattform. */
  objects.push_back({{550, 320}, {590, 370}, {420, 320}});    /* Plattform. */
  objects.push_back({{1150, 780}, {1270, 780}, {1270, 470}}); /* Steep ramp. */
}

GameCharacter &Game::getGameCharacter() { return objects.front(); }

void Game::integratePhysics() {
  auto &object = objects.front();

  object.update();
  object.addVelocityOffset(object.getVelocity());

  for (size_t i = 1; i < objects.size(); ++i) {
    auto &other_object = objects[i];

    const auto displacement_vector =
        object.getBoundingPolygon().collidesWith(other_object.getBoundingPolygon());
    if (displacement_vector) {
      object.handleCollisionWith(other_object, *displacement_vector);
    }
  }
}

void Game::render(SDL_Renderer *renderer) const {
  const auto &game_character = objects.front();

  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (size_t index = 1; index < objects.size(); ++index) {
    renderPolygon(renderer, objects[index].getBoundingPolygon());
  }

  if (game_character.isTouchingGround()) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  } else if (game_character.isTouchingWall()) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
  }
  renderPolygon(renderer, game_character.getBoundingPolygon());

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_RenderDrawLine(renderer, game_character.getBoundingPolygon().getPosition().x,
                     game_character.getBoundingPolygon().getPosition().y,
                     (game_character.getBoundingPolygon().getPosition() +
                      game_character.getRightDirection() * 50.0f)
                         .x,
                     (game_character.getBoundingPolygon().getPosition() +
                      game_character.getRightDirection() * 50.0f)
                         .y);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(
      renderer, game_character.getBoundingPolygon().getPosition().x,
      game_character.getBoundingPolygon().getPosition().y,
      (game_character.getBoundingPolygon().getPosition() + game_character.getVelocity() * 50.0f).x,
      (game_character.getBoundingPolygon().getPosition() + game_character.getVelocity() * 50.0f).y);
}
} // namespace GameEngine
