/** @file
 * Playground for polygons.
 */

#ifndef GAME_ENGINE_SRC_GAME_HPP
#define GAME_ENGINE_SRC_GAME_HPP

#include "GameEngine/Camera.hpp"
#include "GameEngine/Physics/Integrator.hpp"
#include "GameEngine/Physics/JumpAndRunObject.hpp"
#include "GameEngine/Physics/Object.hpp"
#include <SDL_render.h>
#include <memory>
#include <vector>

namespace GameEngine {
class Game {
public:
  Game(size_t screen_width, size_t screen_height);

  Physics::JumpAndRunObject &getGameCharacter();
  const Physics::JumpAndRunObject &getGameCharacter() const;
  void addStaticBox(glm::vec2 screen_position);
  void addDynamicBox(glm::vec2 screen_position);
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
};
} // namespace GameEngine

#endif
