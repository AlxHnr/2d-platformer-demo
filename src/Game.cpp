/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include "Physics/StaticObject.hpp"

namespace {
using namespace GameEngine;

template <typename T>
std::unique_ptr<T> makeBox(const glm::vec2 &center, const float width, const float height) {
  const glm::vec2 box_half_width = {width / 2, 0};
  const glm::vec2 box_half_height = {0, height / 2};
  return std::make_unique<T>(std::initializer_list<glm::vec2>{
      center - box_half_width + box_half_height, center - box_half_width - box_half_height,
      center + box_half_width - box_half_height, center + box_half_width + box_half_height});
}

std::unique_ptr<Physics::StaticObject> makeStaticObject(std::initializer_list<glm::vec2> vertices) {
  return std::make_unique<Physics::StaticObject>(vertices);
}
} // namespace

namespace GameEngine {
Game::Game(const size_t screen_width, const size_t screen_height)
    : camera{screen_width, screen_height} {
  camera.setPosition({screen_width / 2, screen_height / 2});

  objects.push_back(makeBox<Physics::DynamicObject>({65, 305}, 40, 40));

  objects.push_back(makeStaticObject({{10, 10}, {1270, 10}}));    /* Ceiling. */
  objects.push_back(makeStaticObject({{10, 10}, {10, 780}}));     /* Left wall. */
  objects.push_back(makeStaticObject({{1270, 10}, {1270, 780}})); /* Right wall. */
  objects.push_back(makeStaticObject({{10, 780}, {1270, 780}}));  /* Ground. */

  objects.push_back(makeBox<Physics::StaticObject>({870, 705}, 150, 150));
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

void Game::addStaticBox(const glm::vec2 &screen_position) {
  objects.push_back(
      makeBox<Physics::StaticObject>(camera.toWorldCoordinate(screen_position), 20, 20));
}

void Game::addDynamicBox(const glm::vec2 &screen_position) {
  objects.push_back(
      makeBox<Physics::DynamicObject>(camera.toWorldCoordinate(screen_position), 20, 20));
}

void Game::integratePhysics(const std::chrono::microseconds time_since_last_tick) {
  integrator.integrate(time_since_last_tick, objects);
  camera.setPosition(getGameCharacter().getBoundingPolygon().getPosition());
}

void Game::rotateCamera(float angle) {
  camera_orientation += angle;
  camera.setOrientation(camera_orientation);
}

void Game::scaleCamera(float scaling_factor) {
  camera_zoom += scaling_factor;
  camera.setScale(camera_zoom);
}

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

  const auto character_position = game_character.getBoundingPolygon().getPosition();
  const auto character_on_screen = camera.toScreenCoordinate(character_position);
  const auto right_direction_on_screen_end =
      camera.toScreenCoordinate(character_position + game_character.getRightDirection() * 50.0f);
  const auto velocity_on_screen_end =
      camera.toScreenCoordinate(character_position + game_character.getVelocity() * 50.0f);

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_RenderDrawLine(renderer, character_on_screen.x, character_on_screen.y,
                     right_direction_on_screen_end.x, right_direction_on_screen_end.y);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(renderer, character_on_screen.x, character_on_screen.y,
                     velocity_on_screen_end.x, velocity_on_screen_end.y);
}

void Game::renderPolygon(SDL_Renderer *renderer, const ConvexBoundingPolygon &polygon) const {
  polygon.forEachEdge([&](const glm::vec2 &world_start, const glm::vec2 &world_end) {
    const auto start = camera.toScreenCoordinate(world_start);
    const auto end = camera.toScreenCoordinate(world_end);
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  });
}
} // namespace GameEngine
