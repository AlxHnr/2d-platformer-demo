/** @file
 * Contains a class representing static world geometry.
 */

#ifndef GAME_ENGINE_SRC_STATIC_OBJECT_HPP
#define GAME_ENGINE_SRC_STATIC_OBJECT_HPP

#include "ConvexBoundingPolygon.hpp"
#include "PhysicalObject.hpp"
#include <glm/vec2.hpp>
#include <utility>

namespace GameEngine {
/** Static object making up the wolds geometry. */
class StaticObject : public PhysicalObject {
public:
  /** Construct a solid object from the given vertices.
   *
   * @param vertices Zero or more points representing a convex polygon. If no points are provided,
   * it will behave like a non-existing dummy.
   */
  StaticObject(std::initializer_list<glm::vec2> vertices);

  void update() override;
  const glm::vec2 &getVelocity() const override;
  virtual void addVelocityOffset(const glm::vec2 &) override;
  const ConvexBoundingPolygon &getBoundingPolygon() const override;
  bool handleCollisionWith(PhysicalObject &, const glm::vec2 &) override;

private:
  glm::vec2 velocity{};
  ConvexBoundingPolygon bounding_polygon;
};
} // namespace GameEngine

#endif
