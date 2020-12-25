/** @file
 * Tests class for dealing with convex polygon collisions.
 */

#include <ConvexBoundingPolygon.hpp>
#include <doctest/doctest.h>

namespace {
using namespace GameEngine;

/** Basic counterclockwise quad. */
const ConvexBoundingPolygon quad{
    glm::vec2{-1, 1},
    glm::vec2{-1, -1},
    glm::vec2{1, -1},
    glm::vec2{1, 1},
};
} // namespace

TEST_CASE("Polygon collision with zero vertices") {
  const ConvexBoundingPolygon empty_polygon{};
  REQUIRE_FALSE(empty_polygon.collidesWith(quad));
  REQUIRE_FALSE(quad.collidesWith(empty_polygon));
}

TEST_CASE("Polygon collision check with point (one vertex)") {
  const ConvexBoundingPolygon triangle{
      glm::vec2{-1, 1},
      glm::vec2{-1, -1},
      glm::vec2{1, -1},
  };

  SUBCASE("No collision") {
    const ConvexBoundingPolygon point{glm::vec2{0.5, 0.5}};
    REQUIRE_FALSE(point.collidesWith(triangle));
    REQUIRE_FALSE(triangle.collidesWith(point));
  }

  SUBCASE("Collision") {
    const ConvexBoundingPolygon point{glm::vec2{-0.5, 0}};
    REQUIRE(point.collidesWith(triangle));
    REQUIRE(triangle.collidesWith(point));
  }
}

TEST_CASE("Polygon collision resolving with point (one vertex)") {
  SUBCASE("Move point out of rectangle") {
    const ConvexBoundingPolygon point{glm::vec2{-0.5, 0}};
    const auto displacement = point.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(-0.5));
    REQUIRE(displacement->y == doctest::Approx(0));
  }

  SUBCASE("Move rectangle out of point") {
    const ConvexBoundingPolygon point{glm::vec2{-0.5, 0}};
    const auto displacement = quad.collidesWith(point);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.5));
    REQUIRE(displacement->y == doctest::Approx(0));
  }

  SUBCASE("Move point out of rectangle on Y axis") {
    const ConvexBoundingPolygon point{glm::vec2{0, 0.5}};
    const auto displacement = point.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(0.5));
  }

  SUBCASE("Move point out of rectangle (diagonal offset between centers)") {
    const ConvexBoundingPolygon point{glm::vec2{-0.4, -0.6}};
    const auto displacement = point.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(-0.4));
  }
}

TEST_CASE("Polygon collision resolving with line") {
  const ConvexBoundingPolygon triangle{
      glm::vec2{-1, 1},
      glm::vec2{-1, -1},
      glm::vec2{1, -1},
  };

  SUBCASE("No collision") {
    const ConvexBoundingPolygon line{glm::vec2{0.5, -0.1}, glm::vec2{-0.3, 1.2}};
    REQUIRE_FALSE(triangle.collidesWith(line));
    REQUIRE_FALSE(line.collidesWith(triangle));
  }

  SUBCASE("Move line out of triangle") {
    const ConvexBoundingPolygon line{glm::vec2{-0.4, -0.5}, glm::vec2{-0.9, -1.9}};
    const auto displacement = line.collidesWith(triangle);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(-0.5));
  }

  SUBCASE("Move triangle out of line") {
    const ConvexBoundingPolygon line{glm::vec2{-1.4, 0.1}, glm::vec2{0.5, 1.0}};
    const auto displacement = triangle.collidesWith(line);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.27489));
    REQUIRE(displacement->y == doctest::Approx(-0.5803));
  }
}

TEST_CASE("Polygon collision resolving is independent of edge orientation") {
  SUBCASE("Clockwise triangle") {
    const ConvexBoundingPolygon triangle{
        glm::vec2{1.6, 0.25},
        glm::vec2{2.1, -0.6},
        glm::vec2{0.3, -1.3},
    };
    const auto displacement = triangle.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.26323));
    REQUIRE(displacement->y == doctest::Approx(-0.22077));
  }

  SUBCASE("Counterclockwise triangle") {
    const ConvexBoundingPolygon triangle{
        glm::vec2{1.6, 0.25},
        glm::vec2{0.3, -1.3},
        glm::vec2{2.1, -0.6},
    };
    const auto displacement = triangle.collidesWith(quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.26323));
    REQUIRE(displacement->y == doctest::Approx(-0.22077));
  }
}

TEST_CASE("Polygon collides with itself") { REQUIRE(quad.collidesWith(quad)); }

TEST_CASE("Nested Polygon collision") {
  const ConvexBoundingPolygon rectangle{
      glm::vec2{-0.75, 0.75},
      glm::vec2{-0.75, -0.25},
      glm::vec2{0, -0.25},
      glm::vec2{0, 0.75},
  };
  const auto displacement = rectangle.collidesWith(quad);
  REQUIRE(displacement);
  REQUIRE(displacement->x == doctest::Approx(-1));
  REQUIRE(displacement->y == doctest::Approx(0));
}

TEST_CASE("Collision check stops on first failed overlap check") {
  const ConvexBoundingPolygon line{glm::vec2{-1, 1}, glm::vec2{-1, 2}};
  const ConvexBoundingPolygon point{glm::vec2{1, 1.5}};
  REQUIRE_FALSE(line.collidesWith(point));
  REQUIRE_FALSE(point.collidesWith(line));
}
