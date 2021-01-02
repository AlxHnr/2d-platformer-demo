/** @file
 * Implements a playground for polygons.
 */

#include "Game.hpp"
#include "Physics/StaticObject.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <nonstd/span.hpp>

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

/* Represents an object which may require processing the velocity in multiple substeps for fast
 * objects. */
struct UnprocessedObject {
  Physics::Object *object;
  glm::vec2 direction;
  float remaining_velocity_length;
};

const float max_velocity_length = 50;
const float velocity_length_per_step = 3.5;

/** Apply a single velocity/collision substep to the given object.
 *
 * @param unprocessed_object Object which should be moved by its velocity.
 * @param objects All other objects which may collide with the given moving object.
 *
 * @return True if the object was processed completely. False if some unapplied velocity is
 * remaining.
 */
bool processObject(UnprocessedObject &unprocessed_object,
                   nonstd::span<const std::unique_ptr<Physics::Object>> objects) {
  const auto length_of_this_step =
      glm::min(unprocessed_object.remaining_velocity_length, velocity_length_per_step);
  unprocessed_object.object->addVelocityOffset(unprocessed_object.direction * length_of_this_step);

  for (const auto &other_object : objects) {
    if (other_object.get() == unprocessed_object.object) {
      continue;
    }

    const auto displacement_vector = unprocessed_object.object->getBoundingPolygon().collidesWith(
        other_object->getBoundingPolygon());
    if (!displacement_vector) {
      continue;
    }
    unprocessed_object.object->handleCollisionWith(*other_object, *displacement_vector);
    other_object->handleCollisionWith(*unprocessed_object.object, -*displacement_vector);
  }

  unprocessed_object.remaining_velocity_length -= length_of_this_step;
  return unprocessed_object.remaining_velocity_length < glm::epsilon<float>();
}
} // namespace

namespace GameEngine {
Game::Game() {
  objects.push_back(makeBox<GameCharacter>({65, 305}, 40, 40));

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

GameCharacter &Game::getGameCharacter() {
  return *dynamic_cast<GameCharacter *>(objects.front().get());
}

const GameCharacter &Game::getGameCharacter() const {
  return *dynamic_cast<GameCharacter *>(objects.front().get());
}

void Game::integratePhysics() {
  for (const auto &object : objects) {
    object->update();
  }

  std::vector<UnprocessedObject> unprocessed_objects{};

  for (const auto &object : objects) {
    const auto remaining_velocity_length =
        glm::min(glm::length(object->getVelocity()), max_velocity_length);

    /* Prevent normalizing vector with zero length. */
    const auto direction = remaining_velocity_length > glm::epsilon<float>()
                               ? glm::normalize(object->getVelocity())
                               : glm::vec2{};

    UnprocessedObject unprocessed_object{object.get(), direction, remaining_velocity_length};
    if (!processObject(unprocessed_object, objects)) {
      unprocessed_objects.push_back(unprocessed_object);
    }
  }

  while (!unprocessed_objects.empty()) {
    unprocessed_objects.erase(std::remove_if(unprocessed_objects.begin(), unprocessed_objects.end(),
                                             [&](UnprocessedObject &unprocessed_object) {
                                               return processObject(unprocessed_object, objects);
                                             }),
                              unprocessed_objects.end());
  }
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
