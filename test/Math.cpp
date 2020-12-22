/** @file
 * Tests functions in src/Math.hpp
 */

#include <Math.hpp>
#include <doctest/doctest.h>

TEST_CASE("Traverse polygon using Math::ForEachEdge()") {
  SUBCASE("Zero vertices") {
    GameEngine::Math::forEachEdge(
        {}, [](size_t, const glm::vec2 &, const glm::vec2 &) { REQUIRE(false); });
  }

  SUBCASE("One vertex") {
    GameEngine::Math::forEachEdge(
        {}, [](size_t, const glm::vec2 &, const glm::vec2 &) { REQUIRE(false); });
  }

  SUBCASE("Two vertices") {
    size_t edges_traversed = 0;
    std::array line = {glm::vec2{-5, 12}, glm::vec2{6.5, 11}};
    GameEngine::Math::forEachEdge(
        line, [&](const size_t index, const glm::vec2 &start, const glm::vec2 &end) {
          REQUIRE(index == edges_traversed);
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
    GameEngine::Math::forEachEdge(
        triangle, [&](const size_t index, const glm::vec2 &start, const glm::vec2 &end) {
          REQUIRE(index == edges_traversed);

          if (index == 0) {
            REQUIRE(start.x == doctest::Approx(-5));
            REQUIRE(start.y == doctest::Approx(12));
            REQUIRE(end.x == doctest::Approx(-4));
            REQUIRE(end.y == doctest::Approx(-9));
          } else if (index == 1) {
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
    GameEngine::Math::forEachEdge(
        quad, [&](const size_t index, const glm::vec2 &start, const glm::vec2 &end) {
          REQUIRE(index == edges_traversed);

          if (index == 0) {
            REQUIRE(start.x == doctest::Approx(-5));
            REQUIRE(start.y == doctest::Approx(12));
            REQUIRE(end.x == doctest::Approx(-4));
            REQUIRE(end.y == doctest::Approx(-9));
          } else if (index == 1) {
            REQUIRE(start.x == doctest::Approx(-4));
            REQUIRE(start.y == doctest::Approx(-9));
            REQUIRE(end.x == doctest::Approx(6.5));
            REQUIRE(end.y == doctest::Approx(-11));
          } else if (index == 2) {
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
