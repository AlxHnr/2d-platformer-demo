/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_POLYGON_DEMO_HPP
#define GAME_ENGINE_SRC_POLYGON_DEMO_HPP

#include <SDL_render.h>
#include <array>
#include <chrono>
#include <glm/vec2.hpp>

namespace GameEngine {
class PolygonDemo {
public:
  PolygonDemo();
  void handleFrame(SDL_Renderer *renderer, std::chrono::microseconds time_delta);

private:
  std::array<glm::vec2, 4> rectangle{};
  std::array<glm::vec2, 3> triangle{};
};
} // namespace GameEngine

#endif
