/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "Camera.hpp"
#include "Physics/DynamicObject.hpp"
#include "Physics/Integrator.hpp"
#include "Physics/Object.hpp"
#include <SDL_render.h>
#include <memory>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game(size_t screen_width, size_t screen_height);

  Physics::DynamicObject &getGameCharacter();
  const Physics::DynamicObject &getGameCharacter() const;
  void addStaticBox(const glm::vec2 &screen_position);
  void addDynamicBox(const glm::vec2 &screen_position);
  void integratePhysics(std::chrono::microseconds time_since_last_tick);

  void rotateCamera(float angle);
  void scaleCamera(float scaling_factor);
  void render(SDL_Renderer *renderer) const;

private:
  Camera camera;
  float camera_zoom = 1;
  float camera_orientation = 0;

  Physics::Integrator integrator;
  std::vector<std::unique_ptr<Physics::Object>> objects;

  void renderPolygon(SDL_Renderer *renderer, const ConvexBoundingPolygon &polygon) const;
};
} // namespace GameEngine

#endif
