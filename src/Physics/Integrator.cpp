/** @file
 * Implements a physics integrator.
 */

#include "Physics/Integrator.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace {
using namespace std::chrono_literals;
using namespace GameEngine::Physics;

/** Maximal total length of velocity vector applicable per tick. */
constexpr float velocity_length_max = 2.5;

/** For each tick the velocity vector is divided into substeps to prevent objects from clipping or
 * tunneling trough walls. This value is low enough to prevent a square with a size of 0.3x0.3
 * in-game units from falling trough a line at max speed. */
constexpr float velocity_length_substep = 0.15;

constexpr auto ticks_per_second = 60;

constexpr auto tick_duration = std::chrono::microseconds{1s} / ticks_per_second;

/** Prevents the integrator from locking up by spending too much time compensating for low
 * framerates. */
constexpr auto integration_time_max = tick_duration * 10;

/* Represents an object during a substep. */
struct UnprocessedObject {
  Object *object;
  glm::vec2 direction;
  float remaining_velocity_length;
};

/** Apply a single velocity/collision substep to the given object.
 *
 * @param unprocessed_object Object which should be moved by its velocity.
 * @param objects All other objects which may collide with the given moving object.
 *
 * @return True if the object was processed completely. False if some unapplied velocity is
 * remaining.
 */
bool processObject(UnprocessedObject &unprocessed_object,
                   const std::vector<std::unique_ptr<Object>> &objects) {
  const auto length_of_this_step =
      glm::min(unprocessed_object.remaining_velocity_length, velocity_length_substep);
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

void applyTick(const std::vector<std::unique_ptr<Object>> &objects) {
  for (const auto &object : objects) {
    object->update();
  }

  std::vector<UnprocessedObject> unprocessed_objects{};

  for (const auto &object : objects) {
    const auto remaining_velocity_length =
        glm::min(glm::length(object->getVelocity()), velocity_length_max);

    /* Don't normalize vectors with zero length. */
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
} // namespace

namespace GameEngine::Physics {
void Integrator::integrate(const std::chrono::microseconds duration_of_last_frame,
                           const std::vector<std::unique_ptr<Object>> &objects) {
  auto unprocessed_time =
      std::min(duration_of_last_frame + leftover_time_from_last_tick, integration_time_max);

  while (unprocessed_time >= tick_duration) {
    applyTick(objects);
    unprocessed_time -= tick_duration;
  }

  leftover_time_from_last_tick = unprocessed_time;
}
} // namespace GameEngine::Physics
