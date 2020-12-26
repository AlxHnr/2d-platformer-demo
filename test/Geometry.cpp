/** @file
 * Tests functions in src/Geometry.hpp
 */

#include <Geometry.hpp>
#include <doctest/doctest.h>
#include <glm/geometric.hpp>

TEST_CASE("Traverse polygon using Geometry::ForEachEdge()") {
  SUBCASE("Zero vertices") {
    GameEngine::Geometry::forEachEdge({},
                                      [](const glm::vec2 &, const glm::vec2 &) { REQUIRE(false); });
  }

  SUBCASE("One vertex") {
    GameEngine::Geometry::forEachEdge({},
                                      [](const glm::vec2 &, const glm::vec2 &) { REQUIRE(false); });
  }

  SUBCASE("Two vertices") {
    size_t edges_traversed = 0;
    std::array line = {glm::vec2{-5, 12}, glm::vec2{6.5, 11}};
    GameEngine::Geometry::forEachEdge(line, [&](const glm::vec2 &start, const glm::vec2 &end) {
      REQUIRE(start.x == doctest::Approx(-5));
      REQUIRE(start.y == doctest::Approx(12));
      REQUIRE(end.x == doctest::Approx(6.5));
      REQUIRE(end.y == doctest::Approx(11));
      edges_traversed++;
    });
    REQUIRE(edges_traversed == 1);
  }

  SUBCASE("Three vertices") {
    size_t edges_traversed = 0;
    std::array triangle = {glm::vec2{-5, 12}, glm::vec2{-4, -9}, glm::vec2{6.5, -11}};
    GameEngine::Geometry::forEachEdge(triangle, [&](const glm::vec2 &start, const glm::vec2 &end) {
      if (edges_traversed == 0) {
        REQUIRE(start.x == doctest::Approx(-5));
        REQUIRE(start.y == doctest::Approx(12));
        REQUIRE(end.x == doctest::Approx(-4));
        REQUIRE(end.y == doctest::Approx(-9));
      } else if (edges_traversed == 1) {
        REQUIRE(start.x == doctest::Approx(-4));
        REQUIRE(start.y == doctest::Approx(-9));
        REQUIRE(end.x == doctest::Approx(6.5));
        REQUIRE(end.y == doctest::Approx(-11));
      } else {
        REQUIRE(start.x == doctest::Approx(6.5));
        REQUIRE(start.y == doctest::Approx(-11));
        REQUIRE(end.x == doctest::Approx(-5));
        REQUIRE(end.y == doctest::Approx(12));
      }

      edges_traversed++;
    });
    REQUIRE(edges_traversed == 3);
  }

  SUBCASE("Four vertices") {
    size_t edges_traversed = 0;
    std::array quad = {
        glm::vec2{-5, 12},
        glm::vec2{-4, -9},
        glm::vec2{6.5, -11},
        glm::vec2{5, 9.5},
    };
    GameEngine::Geometry::forEachEdge(quad, [&](const glm::vec2 &start, const glm::vec2 &end) {
      if (edges_traversed == 0) {
        REQUIRE(start.x == doctest::Approx(-5));
        REQUIRE(start.y == doctest::Approx(12));
        REQUIRE(end.x == doctest::Approx(-4));
        REQUIRE(end.y == doctest::Approx(-9));
      } else if (edges_traversed == 1) {
        REQUIRE(start.x == doctest::Approx(-4));
        REQUIRE(start.y == doctest::Approx(-9));
        REQUIRE(end.x == doctest::Approx(6.5));
        REQUIRE(end.y == doctest::Approx(-11));
      } else if (edges_traversed == 2) {
        REQUIRE(start.x == doctest::Approx(6.5));
        REQUIRE(start.y == doctest::Approx(-11));
        REQUIRE(end.x == doctest::Approx(5));
        REQUIRE(end.y == doctest::Approx(9.5));
      } else {
        REQUIRE(start.x == doctest::Approx(5));
        REQUIRE(start.y == doctest::Approx(9.5));
        REQUIRE(end.x == doctest::Approx(-5));
        REQUIRE(end.y == doctest::Approx(12));
      }

      edges_traversed++;
    });
    REQUIRE(edges_traversed == 4);
  }
}
