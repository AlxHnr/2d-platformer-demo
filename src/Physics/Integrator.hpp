/** @file
 * Declares a class for physics integration.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_INTEGRATOR_HPP
#define GAME_ENGINE_SRC_PHYSICS_INTEGRATOR_HPP

#include "Physics/Object.hpp"
#include <memory>
#include <vector>

namespace GameEngine::Physics {
class Integrator {
public:
  void integrate(const std::vector<std::unique_ptr<Object>> &objects);
};
} // namespace GameEngine::Physics

#endif
