/** @file
 * Contains the interface for objects usable by the physics engine.
 */

#ifndef GAME_ENGINE_SRC_PHYSICS_OBJECT_HPP
#define GAME_ENGINE_SRC_PHYSICS_OBJECT_HPP

#include "Geometry.hpp"
#include <glm/vec2.hpp>

namespace GameEngine {
/** Interface for objects which can interact via the physics engine. */
class PhysicsObject {
public:
  /** @return Center of the object in the game world. */
  virtual const glm::vec2 &getPosition() const = 0;

  /** @param new_position New position (center) of the object in the game world. */
  virtual void setPosition(const glm::vec2 &new_position) = 0;

  /** @return Velocity of the object. */
  virtual const glm::vec2 &getVelocity() const = 0;

  /** @param new_velocity New velocity of the object. */
  virtual void setVelocity(const glm::vec2 &new_velocity) = 0;

  /** @return Borders of the object in the game world used for collision detection. */
  virtual Geometry::ConvexPolygonView getBoundingPolygon() const = 0;

  /** Will be called every tick to update the state of the object. */
  virtual void update() = 0;

  /** Gets called when a collision occurred.
   *
   * @param other Object which collided with this object.
   * @param displacement_vector Offset required to move this object out of the other object.
   */
  virtual void handleCollision(PhysicsObject &other, const glm::vec2 &displacement_vector) = 0;
};
} // namespace GameEngine

#endif
