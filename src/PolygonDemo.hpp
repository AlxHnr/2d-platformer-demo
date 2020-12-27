/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_POLYGON_DEMO_HPP
#define GAME_ENGINE_SRC_POLYGON_DEMO_HPP

#include "PolygonObject.hpp"
#include <SDL_render.h>
#include <chrono>

namespace GameEngine {
class PolygonDemo {
public:
  PolygonDemo();
  void handleFrame(SDL_Renderer *renderer, std::chrono::microseconds time_delta);

private:
  PolygonObject triangle;
  PolygonObject rectangle;
};
} // namespace GameEngine

#endif
