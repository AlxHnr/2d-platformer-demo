/** @file
 * Declares a class for physics integration.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_INTEGRATOR_HPP
#define GAME_ENGINE_SRC_PHYSICS_INTEGRATOR_HPP

#include "Physics/Object.hpp"
#include <chrono>
#include <memory>
#include <vector>

namespace GameEngine::Physics {
/** Tick based physics integrator running at a fixed tickrate. Considers leftover time from the
 * previous tick to be independent of the rendering framerate. */
class Integrator {
public:
  /** Advance the state of the given objects, compensating for inconstant framerates. To be called
   * every frame.
   *
   * @param duration_of_last_frame Total time elapsed during the last frame. This includes the
   * previous call of this function.
   * @param objects Will be moved by their velocity and collision-checked against all other objects.
   *
   * @return Value between 0 and 1 representing the amount of unprocessed time remaining for the
   * current frame. Used for rendering intermediate states, where 0.0f refers to the objects state
   * at the previous tick and 1.0f refers to the current state.
   */
  float integrate(std::chrono::microseconds duration_of_last_frame,
                  const std::vector<std::unique_ptr<Object>> &objects);

  /** @param speed_factor Factor determining the speed of the game logic. E.g. 0.5f for half the
   * speed or 2.0f to run twice as fast. If negative will be set to zero. */
  void setSpeedFactor(float speed_factor);

private:
  std::chrono::microseconds leftover_time_from_last_tick{};

  float speed_factor = 1;
};
} // namespace GameEngine::Physics

#endif
