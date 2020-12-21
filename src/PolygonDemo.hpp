/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_POLYGON_DEMO_HPP
#define GAME_ENGINE_SRC_POLYGON_DEMO_HPP

#include <SDL_render.h>
#include <chrono>
#include <glm/vec2.hpp>
#include <vector>

namespace GameEngine {
class PolygonDemo {
public:
  PolygonDemo();
  void handleFrame(SDL_Renderer *renderer, std::chrono::microseconds time_delta);

private:
  std::vector<glm::vec2> rectangle{};
  std::vector<glm::vec2> triangle{};
};
} // namespace GameEngine

#endif
