/** @file
 * Tests geometry helper functions.
 */

#include <Geometry.hpp>
#include <doctest/doctest.h>

TEST_CASE("Traverse polygon using forEachEdge()") {
  using namespace GameEngine::Geometry;

  SUBCASE("Zero vertices") {
    forEachEdge({}, [](glm::vec2, glm::vec2) { REQUIRE(false); });
  }

  SUBCASE("One vertex") {
    forEachEdge({{1, 21}}, [](glm::vec2, glm::vec2) { REQUIRE(false); });
  }

  SUBCASE("Two vertices") {
    size_t edges_traversed = 0;
    std::vector<glm::vec2> line{{-5, 12}, {6.5, 11}};
    forEachEdge(line, [&](const glm::vec2 start, const glm::vec2 end) {
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
    std::vector<glm::vec2> triangle{{-5, 12}, {-4, -9}, {6.5, -11}};
    forEachEdge(triangle, [&](const glm::vec2 start, const glm::vec2 end) {
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
    std::vector<glm::vec2> uneven_quad = {{-5, 12}, {-4, -9}, {6.5, -11}, {5, 9.5}};
    forEachEdge(uneven_quad, [&](const glm::vec2 start, const glm::vec2 end) {
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
