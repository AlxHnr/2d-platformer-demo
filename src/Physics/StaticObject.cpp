/** @file
 * Implements a class representing static world geometry.
 */

#include "Physics/StaticObject.hpp"

namespace GameEngine::Physics {
StaticObject::StaticObject(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {}

void StaticObject::update() {}

const glm::vec2 &StaticObject::getVelocity() const { return velocity; }

void StaticObject::addVelocityOffset(const glm::vec2 &) { return; }

const ConvexBoundingPolygon &StaticObject::getBoundingPolygon() const { return bounding_polygon; }

void StaticObject::handleCollisionWith(Physics::Object &, const glm::vec2 &) {}
} // namespace GameEngine::Physics
