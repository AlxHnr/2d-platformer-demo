/** @file
 * Implements a class representing static world geometry.
 */

#include "Physics/StaticObject.hpp"
#include "Geometry.hpp"

namespace GameEngine::Physics {
StaticObject::StaticObject(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {}

void StaticObject::update() {}

glm::vec2 StaticObject::getVelocity() const { return velocity; }

void StaticObject::addVelocityOffset(glm::vec2) { return; }

const ConvexBoundingPolygon &StaticObject::getBoundingPolygon() const { return bounding_polygon; }

void StaticObject::handleCollisionWith(Physics::Object &, glm::vec2) {}

void StaticObject::render(SDL_Renderer *renderer, const Camera &camera, float) const {
  SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
  Geometry::forEachEdge(bounding_polygon.getVertices(),
                        [&](const glm::vec2 world_start, const glm::vec2 world_end) {
                          const auto start = camera.toScreenCoordinate(world_start);
                          const auto end = camera.toScreenCoordinate(world_end);
                          SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
                        });
}
} // namespace GameEngine::Physics
