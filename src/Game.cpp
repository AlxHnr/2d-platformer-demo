/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include "Geometry.hpp"
#include "Physics/StaticObject.hpp"

using namespace GameEngine;

namespace {
template <typename T>
std::unique_ptr<T> makeBox(const glm::vec2 center, const float width, const float height) {
  const glm::vec2 box_half_width = {width / 2, 0};
  const glm::vec2 box_half_height = {0, height / 2};
  return std::make_unique<T>(std::initializer_list<glm::vec2>{
      center - box_half_width - box_half_height, center - box_half_width + box_half_height,
      center + box_half_width + box_half_height, center + box_half_width - box_half_height});
}

std::unique_ptr<Physics::StaticObject> makeStaticObject(std::initializer_list<glm::vec2> vertices) {
  return std::make_unique<Physics::StaticObject>(vertices);
}
} // namespace

namespace GameEngine {
Game::Game(const size_t screen_width, const size_t screen_height)
    : camera{screen_width, screen_height} {
  objects.push_back(makeBox<Physics::JumpAndRunObject>({1.625, -7.625}, 1.0, 1.0));
  camera.setPosition(getGameCharacter().getBoundingPolygon().getPosition());

  objects.push_back(makeStaticObject({{0.25, 100}, {31.75, 100}}));     /* Ceiling. */
  objects.push_back(makeStaticObject({{0.25, 100}, {0.25, -19.5}}));    /* Left wall. */
  objects.push_back(makeStaticObject({{31.75, 100}, {31.75, -19.5}}));  /* Right wall. */
  objects.push_back(makeStaticObject({{0.25, -19.5}, {31.75, -19.5}})); /* Ground. */

  objects.push_back(makeBox<Physics::StaticObject>({21.75, -17.625}, 3.75, 3.75));
  for (size_t index = 0; index < 24; ++index) {
    const float width = 0.375;
    objects.push_back(
        makeStaticObject({{18.625 + width * index, -8.0}, {18.625 + width * index, -8.125}}));
  }

  objects.push_back(makeStaticObject({{18, 2}, {23, -3.0}, {27, 2}})); /* Plattform. */

  objects.push_back(
      makeStaticObject({{11.25, -19.5}, {16.25, -19.5}, {19.875, -15.75}}));         /* Ramp. */
  objects.push_back(makeStaticObject({{0.25, -15.0}, {0.25, -19.5}, {8.5, -19.5}})); /* Ramp. */
  objects.push_back(
      makeStaticObject({{18.75, -11.75}, {19.75, -13.0}, {15.5, -11.75}})); /* Plattform. */
  objects.push_back(
      makeStaticObject({{13.75, -8.0}, {14.75, -9.25}, {10.5, -8.0}})); /* Plattform. */
  objects.push_back(
      makeStaticObject({{28.75, -19.5}, {31.75, -19.5}, {31.75, -11.75}})); /* Steep ramp. */
}

Physics::JumpAndRunObject &Game::getGameCharacter() {
  return *dynamic_cast<Physics::JumpAndRunObject *>(objects.front().get());
}

const Physics::JumpAndRunObject &Game::getGameCharacter() const {
  return *dynamic_cast<Physics::JumpAndRunObject *>(objects.front().get());
}

void Game::addStaticBox(const glm::vec2 screen_position) {
  objects.push_back(
      makeBox<Physics::StaticObject>(camera.toWorldCoordinate(screen_position), 0.5, 0.5));
}

void Game::addDynamicBox(const glm::vec2 screen_position) {
  objects.push_back(
      makeBox<Physics::DynamicObject>(camera.toWorldCoordinate(screen_position), 0.5, 0.5));
}

void Game::integratePhysics(const std::chrono::microseconds time_since_last_tick) {
  integrator.integrate(time_since_last_tick, objects);
  camera.stepTowardsPosition(getGameCharacter().getBoundingPolygon().getPosition());
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
  for (const auto &object : objects) {
    object->render(renderer, camera, 1.0);
  }
}
} // namespace GameEngine
