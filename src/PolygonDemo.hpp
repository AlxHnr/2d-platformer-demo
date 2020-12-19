/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_POLYGON_DEMO_HPP
#define GAME_ENGINE_SRC_POLYGON_DEMO_HPP

#include <SDL_render.h>
#include <array>
#include <chrono>

namespace GameEngine {
class PolygonDemo {
public:
  PolygonDemo();
  void handleFrame(SDL_Renderer *renderer, std::chrono::microseconds time_delta);

  struct Point {
    int x, y;
  };

private:
  std::array<Point, 4> rectangle{};
  std::array<Point, 3> triangle{};
};
} // namespace GameEngine

#endif
