/** @file
 * Tests functions in src/Geometry.hpp
 */

#include <Geometry.hpp>
#include <doctest/doctest.h>
#include <glm/geometric.hpp>

TEST_CASE("Traverse polygon using Geometry::ForEachEdge()") {
  SUBCASE("Zero vertices") {
    GameEngine::Geometry::forEachEdge(
        {}, [](size_t, const glm::vec2 &, const glm::vec2 &) { REQUIRE(false); });
  }

  SUBCASE("One vertex") {
    GameEngine::Geometry::forEachEdge(
        {}, [](size_t, const glm::vec2 &, const glm::vec2 &) { REQUIRE(false); });
  }

  SUBCASE("Two vertices") {
    size_t edges_traversed = 0;
    std::array line = {glm::vec2{-5, 12}, glm::vec2{6.5, 11}};
    GameEngine::Geometry::forEachEdge(
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
    GameEngine::Geometry::forEachEdge(
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
    GameEngine::Geometry::forEachEdge(
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

TEST_CASE("Geometry::computeNormalOfEdge()") {
  const auto normal1 = GameEngine::Geometry::computeNormalOfEdge({0, 0}, {0, 1});
  REQUIRE(normal1.x == doctest::Approx(-1));
  REQUIRE(normal1.y == doctest::Approx(0));

  const auto normal2 = GameEngine::Geometry::computeNormalOfEdge({0, 0}, {1, 0});
  REQUIRE(normal2.x == doctest::Approx(0));
  REQUIRE(normal2.y == doctest::Approx(1));

  const auto normal3 = GameEngine::Geometry::computeNormalOfEdge({0, 0}, {1, 1});
  REQUIRE(normal3.x == doctest::Approx(-0.7071));
  REQUIRE(normal3.y == doctest::Approx(0.7071));
}

TEST_CASE("Geometry::projectVerticesOntoAxisMinMax()") {
  const auto axis = glm::normalize(glm::vec2{5, 2});

  SUBCASE("No vertices") {
    const auto values = GameEngine::Geometry::projectVerticesOntoAxisMinMax({}, axis);
    REQUIRE(values.axis.x == doctest::Approx(axis.x));
    REQUIRE(values.axis.y == doctest::Approx(axis.y));
    REQUIRE(values.min == doctest::Approx(0));
    REQUIRE(values.max == doctest::Approx(0));
  }

  SUBCASE("One vertex") {
    const std::array vertex = {glm::vec2{2, 3}};
    const auto values = GameEngine::Geometry::projectVerticesOntoAxisMinMax(vertex, axis);
    REQUIRE(values.axis.x == doctest::Approx(axis.x));
    REQUIRE(values.axis.y == doctest::Approx(axis.y));
    REQUIRE(values.min == doctest::Approx(2.9711));
    REQUIRE(values.max == doctest::Approx(2.9711));
  }

  SUBCASE("Two vertices") {
    const std::array line = {glm::vec2{2, 3}, glm::vec2{-0.85, 7}};
    const auto values = GameEngine::Geometry::projectVerticesOntoAxisMinMax(line, axis);
    REQUIRE(values.axis.x == doctest::Approx(axis.x));
    REQUIRE(values.axis.y == doctest::Approx(axis.y));
    REQUIRE(values.min == doctest::Approx(1.8105));
    REQUIRE(values.max == doctest::Approx(2.9711));
  }

  SUBCASE("Three vertices") {
    const std::array triangle = {glm::vec2{2, 3}, glm::vec2{-0.85, 7}, glm::vec2{-0.5, 4.2}};
    const auto values = GameEngine::Geometry::projectVerticesOntoAxisMinMax(triangle, axis);
    REQUIRE(values.axis.x == doctest::Approx(axis.x));
    REQUIRE(values.axis.y == doctest::Approx(axis.y));
    REQUIRE(values.min == doctest::Approx(1.0956));
    REQUIRE(values.max == doctest::Approx(2.9711));
  }
}
