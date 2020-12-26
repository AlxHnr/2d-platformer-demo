/** @file
 * Tests convex polygon collisions.
 */

#include <Geometry.hpp>
#include <doctest/doctest.h>

namespace {
using namespace GameEngine::Geometry;

/** Basic counterclockwise quad. */
const std::array quad{
    glm::vec2{-1, 1},
    glm::vec2{-1, -1},
    glm::vec2{1, -1},
    glm::vec2{1, 1},
};
} // namespace

TEST_CASE("Polygon collision with zero vertices") {
  const std::array<glm::vec2, 0> empty_polygon{};
  REQUIRE_FALSE(checkCollision(empty_polygon, quad));
  REQUIRE_FALSE(checkCollision(quad, empty_polygon));
}

TEST_CASE("Polygon collision check with point (one vertex)") {
  const std::array triangle{
      glm::vec2{-1, 1},
      glm::vec2{-1, -1},
      glm::vec2{1, -1},
  };

  SUBCASE("No collision") {
    const std::array point{glm::vec2{0.5, 0.5}};
    REQUIRE_FALSE(checkCollision(point, triangle));
    REQUIRE_FALSE(checkCollision(triangle, point));
  }

  SUBCASE("Collision") {
    const std::array point{glm::vec2{-0.5, 0}};
    REQUIRE(checkCollision(point, triangle));
    REQUIRE(checkCollision(triangle, point));
  }
}

TEST_CASE("Polygon collision resolving with point (one vertex)") {
  SUBCASE("Move point out of rectangle") {
    const std::array point{glm::vec2{-0.5, 0}};
    const auto displacement = checkCollision(point, quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(-0.5));
    REQUIRE(displacement->y == doctest::Approx(0));
  }

  SUBCASE("Move rectangle out of point") {
    const std::array point{glm::vec2{-0.5, 0}};
    const auto displacement = checkCollision(quad, point);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.5));
    REQUIRE(displacement->y == doctest::Approx(0));
  }

  SUBCASE("Move point out of rectangle on Y axis") {
    const std::array point{glm::vec2{0, 0.5}};
    const auto displacement = checkCollision(point, quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(0.5));
  }

  SUBCASE("Move point out of rectangle (diagonal offset between centers)") {
    const std::array point{glm::vec2{-0.4, -0.6}};
    const auto displacement = checkCollision(point, quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(-0.4));
  }
}

TEST_CASE("Polygon collision resolving with line") {
  const std::array triangle{
      glm::vec2{-1, 1},
      glm::vec2{-1, -1},
      glm::vec2{1, -1},
  };

  SUBCASE("No collision") {
    const std::array line{glm::vec2{0.5, -0.1}, glm::vec2{-0.3, 1.2}};
    REQUIRE_FALSE(checkCollision(triangle, line));
    REQUIRE_FALSE(checkCollision(line, triangle));
  }

  SUBCASE("Move line out of triangle") {
    const std::array line{glm::vec2{-0.4, -0.5}, glm::vec2{-0.9, -1.9}};
    const auto displacement = checkCollision(line, triangle);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0));
    REQUIRE(displacement->y == doctest::Approx(-0.5));
  }

  SUBCASE("Move triangle out of line") {
    const std::array line{glm::vec2{-1.4, 0.1}, glm::vec2{0.5, 1.0}};
    const auto displacement = checkCollision(triangle, line);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.27489));
    REQUIRE(displacement->y == doctest::Approx(-0.5803));
  }
}

TEST_CASE("Polygon collision resolving is independent of edge orientation") {
  SUBCASE("Clockwise triangle") {
    const std::array triangle{
        glm::vec2{1.6, 0.25},
        glm::vec2{2.1, -0.6},
        glm::vec2{0.3, -1.3},
    };
    const auto displacement = checkCollision(triangle, quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.26323));
    REQUIRE(displacement->y == doctest::Approx(-0.22077));
  }

  SUBCASE("Counterclockwise triangle") {
    const std::array triangle{
        glm::vec2{1.6, 0.25},
        glm::vec2{0.3, -1.3},
        glm::vec2{2.1, -0.6},
    };
    const auto displacement = checkCollision(triangle, quad);
    REQUIRE(displacement);
    REQUIRE(displacement->x == doctest::Approx(0.26323));
    REQUIRE(displacement->y == doctest::Approx(-0.22077));
  }
}

TEST_CASE("Polygon collides with itself") { REQUIRE(checkCollision(quad, quad)); }

TEST_CASE("Nested Polygon collision") {
  const std::array rectangle{
      glm::vec2{-0.75, 0.75},
      glm::vec2{-0.75, -0.25},
      glm::vec2{0, -0.25},
      glm::vec2{0, 0.75},
  };
  const auto displacement = checkCollision(rectangle, quad);
  REQUIRE(displacement);
  REQUIRE(displacement->x == doctest::Approx(-1));
  REQUIRE(displacement->y == doctest::Approx(0));
}

TEST_CASE("Collision check stops on first failed overlap check") {
  const std::array line{glm::vec2{-1, 1}, glm::vec2{-1, 2}};
  const std::array point{glm::vec2{1, 1.5}};
  REQUIRE_FALSE(checkCollision(line, point));
  REQUIRE_FALSE(checkCollision(point, line));
}
