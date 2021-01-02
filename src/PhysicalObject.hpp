/** @file
 * Contains an interface for objects which interact with the physics engine.
 */

#ifndef GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICAL_OBJECT_HPP

#include "ConvexBoundingPolygon.hpp"
#include <glm/vec2.hpp>

namespace GameEngine {
/** Represents an object which can move and collide with other objects. */
class PhysicalObject {
public:
  virtual ~PhysicalObject() = default;

  /** Update the state of the object for the current tick. This function does not need to apply the
   * objects velocity to its position, which is done by the physics integrator. Will be called only
   * once per tick, before all addVelocityStep() and handleCollisionWith() calls.
   */
  virtual void update() = 0;

  /** @return Current velocity of this object. Will be applied by the physics engine. */
  virtual const glm::vec2 &getVelocity() const = 0;

  /** Move this object by the given amount. May be called multiple times for each tick to handle
   * substeps. Must update the polygon returned by getBoundingPolygon().
   *
   * @param Offset Offset to be added to the objects current position.
   */
  virtual void addVelocityOffset(const glm::vec2 &offset) = 0;

  virtual const ConvexBoundingPolygon &getBoundingPolygon() const = 0;

  /** Will be called if a collision occurred. Two objects may collide multiple times during the same
   * tick.
   *
   * @param other Object which this object collided with.
   * @param displacement_vector Offset for moving this object out of the other object.
   */
  virtual void handleCollisionWith(PhysicalObject &other, const glm::vec2 &displacement_vector) = 0;
};
} // namespace GameEngine

#endif
