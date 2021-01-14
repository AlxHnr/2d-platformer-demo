/** @file
 * Implements a basic polygon class.
 */

#include "ConvexBoundingPolygon.hpp"
#include "Geometry.hpp"
#include <SDL_assert.h>
#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <numeric>

namespace {
using namespace GameEngine;

glm::vec2 computeCenter(const std::vector<glm::vec2> &polygon) {
  return std::accumulate(polygon.cbegin(), polygon.cend(), glm::vec2{}) /
         static_cast<float>(polygon.size());
}

/** @return Normal vector orthogonal to the polygons nth edge. */
glm::vec2 getEdgeNormal(const std::vector<glm::vec2> &polygon, const size_t edge_index) {
  const auto [start, end] = Geometry::getEdge(polygon, edge_index);
  return glm::normalize(glm::vec2{start.y - end.y, end.x - start.x});
}

/** Contains the smallest and largest values found while projecting vertices onto an axis. */
struct ProjectedVertices {
  glm::vec2 axis; /**< Normalized axis onto which vertices got projected. */
  float min;      /**< Smallest projected value. */
  float max;      /**< Largest projected value. */
};

ProjectedVertices projectVerticesOntoAxis(const std::vector<glm::vec2> &polygon,
                                          const glm::vec2 axis) {
  const auto first_dot_product = glm::dot(polygon.front(), axis);
  float min = first_dot_product;
  float max = first_dot_product;

  for (size_t index = 1; index < polygon.size(); ++index) {
    const auto dot_product = glm::dot(polygon[index], axis);
    min = glm::min(min, dot_product);
    max = glm::max(max, dot_product);
  }
  return {axis, min, max};
}

/** @return Overlap found while projecting the given polygons onto the specified axis. Will be < 0
 * if no overlap exists. */
float getProjectionOverlap(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b,
                           const glm::vec2 axis) {
  const auto a_projected = projectVerticesOntoAxis(a, axis);
  const auto b_projected = projectVerticesOntoAxis(b, axis);
  return glm::min(b_projected.max - a_projected.min, a_projected.max - b_projected.min);
}

/** Offset for moving one polygon out of another. */
struct DisplacementVector {
  glm::vec2 direction;
  float magnitude;
};

/** @return Smallest displacement vector (MTV) for moving polygon a out of polygon b. Will return
 * nothing if no collision occurred. */
std::optional<DisplacementVector> findSmallestDisplacementVector(const std::vector<glm::vec2> &a,
                                                                 const std::vector<glm::vec2> &b) {
  /** Use X axis as direction for polygons with only one vertex. */
  auto direction_of_smallest_overlap = a.size() == 1 ? glm::vec2{1, 0} : getEdgeNormal(a, 0);
  auto smallest_overlap = getProjectionOverlap(a, b, direction_of_smallest_overlap);
  if (smallest_overlap <= glm::epsilon<float>()) {
    return std::nullopt;
  }

  const auto a_edges = Geometry::countEdges(a);
  for (size_t index = 1; index < a_edges; ++index) {
    const auto axis = getEdgeNormal(a, index);
    const auto overlap = getProjectionOverlap(a, b, axis);
    if (overlap <= glm::epsilon<float>()) {
      return std::nullopt;
    }
    if (overlap < smallest_overlap) {
      smallest_overlap = overlap;
      direction_of_smallest_overlap = axis;
    }
  }

  return DisplacementVector{direction_of_smallest_overlap, smallest_overlap};
}
} // namespace

namespace GameEngine {
ConvexBoundingPolygon::ConvexBoundingPolygon(std::initializer_list<glm::vec2> vertices)
    : bounding_polygon{vertices} {
  position = vertices.size() == 0 ? glm::vec2{0, 0} : computeCenter(vertices);
  std::transform(vertices.begin(), vertices.end(),
                 std::back_inserter(bounding_polygon_relative_to_center),
                 [this](const glm::vec2 vertex) { return vertex - position; });
}

glm::vec2 ConvexBoundingPolygon::getPosition() const { return position; }

void ConvexBoundingPolygon::setPosition(const glm::vec2 position) {
  this->position = position;
  recomputeBoundingPolygon();
}

float ConvexBoundingPolygon::getOrientation() const { return orientation; }

void ConvexBoundingPolygon::setOrientation(const float orientation) {
  this->orientation = glm::mod(orientation, glm::two_pi<float>());
  recomputeBoundingPolygon();
}

std::optional<glm::vec2>
ConvexBoundingPolygon::collidesWith(const ConvexBoundingPolygon &other) const {
  if (this->bounding_polygon.empty() || other.bounding_polygon.empty()) {
    return std::nullopt;
  }

  const auto displacement_this_from_other =
      findSmallestDisplacementVector(this->bounding_polygon, other.bounding_polygon);
  if (!displacement_this_from_other) {
    return std::nullopt;
  }

  const auto displacement_other_from_this =
      findSmallestDisplacementVector(other.bounding_polygon, this->bounding_polygon);
  if (!displacement_other_from_this) {
    return std::nullopt;
  }

  const auto displacement_vector = [&] {
    if (displacement_this_from_other->magnitude < displacement_other_from_this->magnitude) {
      SDL_assert(displacement_this_from_other->magnitude > glm::epsilon<float>());
      return displacement_this_from_other->direction * displacement_this_from_other->magnitude;
    }
    SDL_assert(displacement_other_from_this->magnitude > glm::epsilon<float>());
    return -displacement_other_from_this->direction * displacement_other_from_this->magnitude;
  }();

  const auto direction_from_a_to_b =
      computeCenter(other.bounding_polygon) - computeCenter(this->bounding_polygon);
  if (glm::dot(direction_from_a_to_b, displacement_vector) < 0) {
    return displacement_vector;
  }
  return -displacement_vector;
}

void ConvexBoundingPolygon::recomputeBoundingPolygon() {
  std::transform(
      bounding_polygon_relative_to_center.cbegin(), bounding_polygon_relative_to_center.cend(),
      bounding_polygon.begin(),
      [this](const glm::vec2 vertex) { return glm::rotate(vertex, orientation) + position; });
}

const std::vector<glm::vec2> &ConvexBoundingPolygon::getVertices() const {
  return bounding_polygon;
}
} // namespace GameEngine
