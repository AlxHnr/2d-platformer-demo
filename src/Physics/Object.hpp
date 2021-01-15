/** @file
 * Contains an interface for objects which interact with the physics engine.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICS_OBJECT_HPP

#include "ConvexBoundingPolygon.hpp"
#include "Renderable.hpp"
#include <glm/vec2.hpp>

namespace GameEngine::Physics {
/** Represents an object which can move and collide with other objects. */
class Object : public Renderable {
public:
  virtual ~Object() = default;

  /** Update the state of the object including its velocity vector. This function should not apply
   * the velocity. Will be called once at the beginning of each tick. Will be called before
   * addVelocityStep() and handleCollisionWith().  */
  virtual void update() = 0;

  /** @return Current velocity of this object. Will be applied by the physics engine. */
  virtual glm::vec2 getVelocity() const = 0;

  /** Move this object by the given amount. May be called multiple times for each tick to handle
   * substeps. Must update the polygon returned by getBoundingPolygon().
   *
   * @param offset To be added to the objects current position.
   */
  virtual void addVelocityOffset(glm::vec2 offset) = 0;

  /** @return Boundaries of this object in the game world for collision detection. */
  virtual const ConvexBoundingPolygon &getBoundingPolygon() const = 0;

  /** Will be called if a collision occurred. Two objects may collide multiple times during the same
   * tick.
   *
   * @param other Object which this object collided with.
   * @param displacement_vector Offset for moving this object out of the other object.
   */
  virtual void handleCollisionWith(Object &other, glm::vec2 displacement_vector) = 0;
};
} // namespace GameEngine::Physics

#endif
