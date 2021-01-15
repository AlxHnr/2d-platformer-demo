/** @file
 * Contains an interface for renderable objects.
 */

#ifndef GAME_ENGINE_SRC_RENDERABLE_HPP
#define GAME_ENGINE_SRC_RENDERABLE_HPP

#include "Camera.hpp"
#include <SDL_render.h>

namespace GameEngine {
/** Represents objects which can be rendered to the screen. */
class Renderable {
public:
  virtual ~Renderable() = default;

  /** Render this object to the screen.
   *
   * @param renderer SDL renderer to use.
   * @param camera Transforms game-world coordinates to screen coordinates.
   * @param integrator_tick_blend_factor Value between 0 and 1. Used for interpolating between the
   * objects state at the previous tick and its current state, where 0 means the previous state.
   */
  virtual void render(SDL_Renderer *renderer, const Camera &camera,
                      float integrator_tick_blend_factor) const = 0;
};
} // namespace GameEngine

#endif
