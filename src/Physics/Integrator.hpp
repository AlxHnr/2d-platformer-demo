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
/** Tick based physics integrator. Runs at a fixed tickrate and uses a fixed time delta. Considers
 * leftover time from the previous tick to be independent of the rendering framerate. */
class Integrator {
public:
  /** Apply one thick to the given objects.
   *
   * @param duration_of_last_frame Total time elapsed during the last frame. This includes the
   * previous call of this function.
   * @param objects Will be moved by their velocity and collision-checked against all other objects.
   */
  void integrate(std::chrono::microseconds duration_of_last_frame,
                 const std::vector<std::unique_ptr<Object>> &objects);

private:
  std::chrono::microseconds leftover_time_from_last_tick;
};
} // namespace GameEngine::Physics

#endif
