/** @file
 * Contains an interface for objects which interact with the physics engine.
 */

#ifndef GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP

#include "ConvexBoundingPolygon.hpp"
#include <glm/vec2.hpp>

namespace GameEngine {
class PhysicalObject {
  /** Update the state of the object. This function does not need to apply the objects velocity to
   * its position. This will be done by the physics integrator. Will be called only once per tick.
   * Will be called once before all addVelocityStep() and handleCollisionWith() calls of this tick.
   */
  virtual void update() = 0;

  /** @return Current velocity of this object. Will be applied by the physics engine. */
  virtual const glm::vec2 &getVelocity() const = 0;

  /** Move this object by the given amount. Must update the polygon returned by
   * getBoundingPolygon(). May be called multiple times for multisampling during physics
   * integration.
   *
   * @param Offset Offset to be added to the objects current position.
   */
  virtual void addVelocityOffset(const glm::vec2 &offset) = 0;

  virtual const ConvexBoundingPolygon &getBoundingPolygon() const = 0;

  /** Will be called if a collision occurred.
   *
   * @param other Object which collided with this object.
   * @param displacement_vector Offset for moving this object out of the other object.
   *
   * @return False to stop the physics engine from applying velocity/movement, e.g. object has hit a
   * wall. This is used to stop multisampling during collision detection.
   */
  virtual bool handleCollisionWith(PhysicalObject &other, const glm::vec2 &displacement_vector) = 0;
};
} // namespace GameEngine

#endif
