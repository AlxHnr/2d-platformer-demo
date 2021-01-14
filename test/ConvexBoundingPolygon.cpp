/** @file
 * Tests ConvexBoundingPolygon class.
 */

#include <ConvexBoundingPolygon.hpp>
#include <doctest/doctest.h>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

namespace {
using namespace GameEngine;

/** Basic counterclockwise quad. */
const ConvexBoundingPolygon quad{{-1, 1}, {-1, -1}, {1, -1}, {1, 1}};
} // namespace

TEST_CASE("Compute center of polygons") {
  SUBCASE("Polygon with one vertex") {
    const auto center = ConvexBoundingPolygon{{12, 1}}.getPosition();
    REQUIRE(center.x == doctest::Approx(12));
    REQUIRE(center.y == doctest::Approx(1));
  }

  SUBCASE("Polygon with two vertices") {
    const auto center = ConvexBoundingPolygon{{-1, 1}, {1, -1}}.getPosition();
    REQUIRE(center.x == doctest::Approx(0));
    REQUIRE(center.y == doctest::Approx(0));
  }

  SUBCASE("Polygon with four vertices") {
    const auto center = quad.getPosition();
    REQUIRE(center.x == doctest::Approx(0));
    REQUIRE(center.y == doctest::Approx(0));
  }
}

TEST_CASE("Update position of polygon") {
  SUBCASE("Polygon with two vertices") {
    ConvexBoundingPolygon line{{-1, 1}, {1, -1}};
    line.setPosition({2, 3});
    REQUIRE(line.getPosition().x == doctest::Approx(2));
    REQUIRE(line.getPosition().y == doctest::Approx(3));
    REQUIRE(line.getVertices().at(0).x == doctest::Approx(1));
    REQUIRE(line.getVertices().at(0).y == doctest::Approx(4));
    REQUIRE(line.getVertices().at(1).x == doctest::Approx(3));
    REQUIRE(line.getVertices().at(1).y == doctest::Approx(2));
  }

  SUBCASE("Polygon with four vertices") {
    ConvexBoundingPolygon moved_quad = quad;
    moved_quad.setPosition({10, 20});
    REQUIRE(moved_quad.getPosition().x == doctest::Approx(10));
    REQUIRE(moved_quad.getPosition().y == doctest::Approx(20));
    REQUIRE(moved_quad.getVertices().at(0).x == doctest::Approx(9));
    REQUIRE(moved_quad.getVertices().at(0).y == doctest::Approx(21));
    REQUIRE(moved_quad.getVertices().at(1).x == doctest::Approx(9));
    REQUIRE(moved_quad.getVertices().at(1).y == doctest::Approx(19));
    REQUIRE(moved_quad.getVertices().at(2).x == doctest::Approx(11));
    REQUIRE(moved_quad.getVertices().at(2).y == doctest::Approx(19));
    REQUIRE(moved_quad.getVertices().at(3).x == doctest::Approx(11));
    REQUIRE(moved_quad.getVertices().at(3).y == doctest::Approx(21));
  }
}

TEST_CASE("Get and set orientation of polygon") {
  ConvexBoundingPolygon rotated_quad = quad;
  REQUIRE(rotated_quad.getOrientation() == doctest::Approx(0));

  rotated_quad.setOrientation(glm::radians(40.0f));
  REQUIRE(rotated_quad.getOrientation() == doctest::Approx(glm::radians(40.0f)));
}

TEST_CASE("Rotate polygon") {
  ConvexBoundingPolygon rotated_quad = quad;
  rotated_quad.setOrientation(glm::radians(45.0f));
  REQUIRE(rotated_quad.getVertices().at(0).x == doctest::Approx(-glm::root_two<float>()));
  REQUIRE(rotated_quad.getVertices().at(0).y == doctest::Approx(0));
  REQUIRE(rotated_quad.getVertices().at(1).x == doctest::Approx(0));
  REQUIRE(rotated_quad.getVertices().at(1).y == doctest::Approx(-glm::root_two<float>()));
  REQUIRE(rotated_quad.getVertices().at(2).x == doctest::Approx(glm::root_two<float>()));
  REQUIRE(rotated_quad.getVertices().at(2).y == doctest::Approx(0));
  REQUIRE(rotated_quad.getVertices().at(3).x == doctest::Approx(0));
  REQUIRE(rotated_quad.getVertices().at(3).y == doctest::Approx(glm::root_two<float>()));
}

TEST_CASE("Polygon collision with zero vertices") {
  const ConvexBoundingPolygon empty_polygon{};
  REQUIRE_FALSE(empty_polygon.collidesWith(quad));
  REQUIRE_FALSE(quad.collidesWith(empty_polygon));
}

TEST_CASE("Polygon collision check with point (one vertex)") {
  const ConvexBoundingPolygon triangle{{-1, 1}, {-1, -1}, {1, -1}};

  SUBCASE("No collision") {
    const ConvexBoundingPolygon point{{0.5, 0.5}};
    REQUIRE_FALSE(point.collidesWith(triangle));
    REQUIRE_FALSE(triangle.collidesWith(point));
  }

  SUBCASE("Collision") {
    const ConvexBoundingPolygon point{{-0.5, 0}};
    REQUIRE(point.collidesWith(triangle));
    REQUIRE(triangle.collidesWith(point));
  }
}

TEST_CASE("Polygon collision resolving with point (one vertex)") {
  SUBCASE("Move point out of rectangle") {
    const ConvexBoundingPolygon point{{-0.5, 0}};
    const auto displacement = point.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(-0.5));
    REQUIRE(displacement->y == doctest::Approx(0));
  }

  SUBCASE("Move rectangle out of point") {
    const ConvexBoundingPolygon point{{-0.5, 0}};
    const auto displacement = quad.collidesWith(point);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.5));
    REQUIRE(displacement->y == doctest::Approx(0));
  }

  SUBCASE("Move point out of rectangle on Y axis") {
    const ConvexBoundingPolygon point{{0, 0.5}};
    const auto displacement = point.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(0.5));
  }

  SUBCASE("Move point out of rectangle (diagonal offset between centers)") {
    const ConvexBoundingPolygon point{{-0.4, -0.6}};
    const auto displacement = point.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(-0.4));
  }
}

TEST_CASE("Polygon collision resolving with line") {
  const ConvexBoundingPolygon triangle{{-1, 1}, {-1, -1}, {1, -1}};

  SUBCASE("No collision") {
    const ConvexBoundingPolygon line{{0.5, -0.1}, {-0.3, 1.2}};
    REQUIRE_FALSE(triangle.collidesWith(line));
    REQUIRE_FALSE(line.collidesWith(triangle));
  }

  SUBCASE("Move line out of triangle") {
    const ConvexBoundingPolygon line{{-0.4, -0.5}, {-0.9, -1.9}};
    const auto displacement = line.collidesWith(triangle);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(-0.5));
  }

  SUBCASE("Move triangle out of line") {
    const ConvexBoundingPolygon line{{-1.4, 0.1}, {0.5, 1.0}};
    const auto displacement = triangle.collidesWith(line);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.27489));
    REQUIRE(displacement->y == doctest::Approx(-0.5803));
  }
}

TEST_CASE("Polygon collision resolving is independent of edge orientation") {
  SUBCASE("Clockwise triangle") {
    const ConvexBoundingPolygon triangle{{1.6, 0.25}, {2.1, -0.6}, {0.3, -1.3}};
    const auto displacement = triangle.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.26323));
    REQUIRE(displacement->y == doctest::Approx(-0.22077));
  }

  SUBCASE("Counterclockwise triangle") {
    const ConvexBoundingPolygon triangle{{1.6, 0.25}, {0.3, -1.3}, {2.1, -0.6}};
    const auto displacement = triangle.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.26323));
    REQUIRE(displacement->y == doctest::Approx(-0.22077));
  }
}

TEST_CASE("Polygon collides with itself") { REQUIRE(quad.collidesWith(quad)); }

TEST_CASE("Nested Polygon collision") {
  const ConvexBoundingPolygon rectangle{{-0.75, 0.75}, {-0.75, -0.25}, {0, -0.25}, {0, 0.75}};
  const auto displacement = rectangle.collidesWith(quad);
  REQUIRE(displacement);
  REQUIRE(displacement->x == doctest::Approx(-1));
  REQUIRE(displacement->y == doctest::Approx(0));
}

TEST_CASE("Collision check stops on first failed overlap check") {
  const ConvexBoundingPolygon line{{-1, 1}, {-1, 2}};
  const ConvexBoundingPolygon point{{1, 1.5}};
  REQUIRE_FALSE(line.collidesWith(point));
  REQUIRE_FALSE(point.collidesWith(line));
}

TEST_CASE("Collision with infinitesimally small displacement vector") {
  const ConvexBoundingPolygon line1{{0, 1}, {0, 2}};
  const ConvexBoundingPolygon line2{{0, 2}, {0, 3}};
  REQUIRE_FALSE(line1.collidesWith(line2));
  REQUIRE_FALSE(line2.collidesWith(line1));
}
