/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include "Physics/StaticObject.hpp"

namespace {
using namespace GameEngine;

void renderPolygon(SDL_Renderer *renderer, const ConvexBoundingPolygon &polygon) {
  polygon.forEachEdge([&](const glm::vec2 &start, const glm::vec2 &end) {
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}

template <typename T>
std::unique_ptr<T> makeBox(const glm::vec2 &center, const float width, const float height) {
  const glm::vec2 box_half_width = {width / 2, 0};
  const glm::vec2 box_half_height = {0, height / 2};
  const glm::vec2 box_center{center.x - box_half_width.x, center.y - box_half_height.y};
  return std::make_unique<T>(std::initializer_list<glm::vec2>{
      box_center - box_half_width + box_half_height, box_center - box_half_width - box_half_height,
      box_center + box_half_width - box_half_height,
      box_center + box_half_width + box_half_height});
}

std::unique_ptr<Physics::StaticObject> makeStaticObject(std::initializer_list<glm::vec2> vertices) {
  return std::make_unique<Physics::StaticObject>(vertices);
}
} // namespace

namespace GameEngine {
Game::Game() {
  objects.push_back(makeBox<Physics::DynamicObject>({65, 305}, 40, 40));

  objects.push_back(makeStaticObject({{10, 10}, {1270, 10}}));    /* Ceiling. */
  objects.push_back(makeStaticObject({{10, 10}, {10, 780}}));     /* Left wall. */
  objects.push_back(makeStaticObject({{1270, 10}, {1270, 780}})); /* Right wall. */
  objects.push_back(makeStaticObject({{10, 780}, {1270, 780}}));  /* Floor. */

  objects.push_back(makeBox<Physics::StaticObject>({945, 780}, 150, 150));
  for (size_t index = 0; index < 24; ++index) {
    const float width = 15;
    objects.push_back(makeStaticObject({{745 + width * index, 320}, {745 + width * index, 325}}));
  }

  objects.push_back(makeStaticObject({{450, 780}, {650, 780}, {795, 630}}));    /* Ramp. */
  objects.push_back(makeStaticObject({{10, 600}, {10, 780}, {340, 780}}));      /* Ramp. */
  objects.push_back(makeStaticObject({{750, 470}, {790, 520}, {620, 470}}));    /* Plattform. */
  objects.push_back(makeStaticObject({{550, 320}, {590, 370}, {420, 320}}));    /* Plattform. */
  objects.push_back(makeStaticObject({{1150, 780}, {1270, 780}, {1270, 470}})); /* Steep ramp. */
}

Physics::DynamicObject &Game::getGameCharacter() {
  return *dynamic_cast<Physics::DynamicObject *>(objects.front().get());
}

const Physics::DynamicObject &Game::getGameCharacter() const {
  return *dynamic_cast<Physics::DynamicObject *>(objects.front().get());
}

void Game::integratePhysics() { integrator.integrate(objects); }

void Game::render(SDL_Renderer *renderer) const {
  const auto &game_character = getGameCharacter();

  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  for (size_t index = 1; index < objects.size(); ++index) {
    renderPolygon(renderer, objects[index]->getBoundingPolygon());
  }

  if (game_character.isTouchingGround()) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  } else if (game_character.isTouchingWall()) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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
