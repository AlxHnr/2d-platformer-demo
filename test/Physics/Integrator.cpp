/** @file
 * Tests the physics integrator.
 */

#include <Physics/Integrator.hpp>
#include <Physics/Object.hpp>
#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <glm/geometric.hpp>

namespace {
using namespace GameEngine;
using namespace std::chrono_literals;

class MockObject : public Physics::Object {
  MAKE_MOCK0(update, void(), override);
  MAKE_CONST_MOCK0(getVelocity, glm::vec2(), override);
  MAKE_MOCK1(addVelocityOffset, void(glm::vec2), override);
  MAKE_CONST_MOCK0(getBoundingPolygon, const ConvexBoundingPolygon &(), override);
  MAKE_MOCK2(handleCollisionWith, void(Physics::Object &, glm::vec2), override);
};
} // namespace

TEST_CASE("Physics::Integrator consumes frame duration in ticks when updating objects") {
  std::vector<std::unique_ptr<Physics::Object>> objects;
  objects.push_back(std::make_unique<MockObject>());
  objects.push_back(std::make_unique<MockObject>());
  const glm::vec2 velocity{1, 0};
  const ConvexBoundingPolygon polygon{};

  auto &first_object = static_cast<MockObject &>(*objects.front());
  auto &second_object = static_cast<MockObject &>(*objects.back());

  ALLOW_CALL(first_object, getVelocity()).RETURN(velocity);
  ALLOW_CALL(first_object, addVelocityOffset(trompeloeil::_));
  ALLOW_CALL(first_object, getBoundingPolygon()).RETURN(polygon);

  ALLOW_CALL(second_object, getVelocity()).RETURN(velocity);
  ALLOW_CALL(second_object, addVelocityOffset(trompeloeil::_));
  ALLOW_CALL(second_object, getBoundingPolygon()).RETURN(polygon);

  SUBCASE("Small frame durations do nothing on initial call") {
    Physics::Integrator{}.integrate(3ms, objects);
  }

  SUBCASE("Only One update call when frame duration equals to tick duration") {
    REQUIRE_CALL(first_object, update());
    REQUIRE_CALL(second_object, update());
    Physics::Integrator{}.integrate(17ms, objects);
  }

  SUBCASE("Long frame durations cause multiple update calls") {
    REQUIRE_CALL(first_object, update()).TIMES(3);
    REQUIRE_CALL(second_object, update()).TIMES(3);
    Physics::Integrator{}.integrate(17ms * 3, objects);
  }

  SUBCASE("Unprocessed time gets carried over to next tick") {
    REQUIRE_CALL(first_object, update()).TIMES(2);
    REQUIRE_CALL(second_object, update()).TIMES(2);

    Physics::Integrator integrator;
    integrator.integrate(30ms, objects);
    integrator.integrate(4ms, objects);
  }

  SUBCASE("Small unprocessed durations get carried over until a full tick is reached") {
    REQUIRE_CALL(first_object, update());
    REQUIRE_CALL(second_object, update());

    Physics::Integrator integrator;
    integrator.integrate(2ms, objects);
    integrator.integrate(10ms, objects);
    integrator.integrate(5ms, objects);
  }

  SUBCASE("Long frame durations get capped") {
    REQUIRE_CALL(first_object, update()).TIMES(10);
    REQUIRE_CALL(second_object, update()).TIMES(10);
    Physics::Integrator{}.integrate(20min, objects);
  }
}

TEST_CASE("Physics::Integrator applies velocity properly") {
  std::vector<std::unique_ptr<Physics::Object>> objects;
  objects.push_back(std::make_unique<MockObject>());
  auto &object = static_cast<MockObject &>(*objects.front());
  const ConvexBoundingPolygon polygon{};

  REQUIRE_CALL(object, update());
  ALLOW_CALL(object, getBoundingPolygon()).RETURN(polygon);

  SUBCASE("Small velocities are applied in one step") {
    const glm::vec2 velocity{0.0375, 0.05};
    ALLOW_CALL(object, getVelocity()).RETURN(velocity);
    REQUIRE_CALL(object, addVelocityOffset(trompeloeil::_))
        .WITH(_1.x == doctest::Approx(0.0375))
        .WITH(_1.y == doctest::Approx(0.05));
    Physics::Integrator{}.integrate(17ms, objects);
  }

  SUBCASE("Large velocities are divided into multiple substeps") {
    const glm::vec2 velocity{-0.343, 0.3};
    const auto direction_step = glm::normalize(velocity) * 0.15f;
    const auto last_step = glm::mod(velocity, direction_step);
    ALLOW_CALL(object, getVelocity()).RETURN(velocity);

    trompeloeil::sequence sequence;
    REQUIRE_CALL(object, addVelocityOffset(trompeloeil::_))
        .IN_SEQUENCE(sequence)
        .WITH(_1.x == doctest::Approx(direction_step.x))
        .WITH(_1.y == doctest::Approx(direction_step.y))
        .TIMES(3);
    REQUIRE_CALL(object, addVelocityOffset(trompeloeil::_))
        .IN_SEQUENCE(sequence)
        .WITH(_1.x == doctest::Approx(last_step.x)) /* Last remaining substep. */
        .WITH(_1.y == doctest::Approx(last_step.y));

    Physics::Integrator{}.integrate(17ms, objects);
  }

  SUBCASE("Applicable velocity has a limit per tick") {
    const glm::vec2 velocity{290, -950};
    const auto direction_step = glm::normalize(velocity) * 0.15f;
    const auto last_step = glm::mod(glm::normalize(velocity) * 2.5f, direction_step);
    ALLOW_CALL(object, getVelocity()).RETURN(velocity);

    trompeloeil::sequence sequence;
    REQUIRE_CALL(object, addVelocityOffset(trompeloeil::_))
        .IN_SEQUENCE(sequence)
        .WITH(_1.x == doctest::Approx(direction_step.x))
        .WITH(_1.y == doctest::Approx(direction_step.y))
        .TIMES(static_cast<size_t>(2.5 / 0.15));
    REQUIRE_CALL(object, addVelocityOffset(trompeloeil::_))
        .IN_SEQUENCE(sequence)
        .WITH(_1.x == doctest::Approx(last_step.x)) /* Last remaining substep. */
        .WITH(_1.y == doctest::Approx(last_step.y));

    Physics::Integrator{}.integrate(17ms, objects);
  }

  SUBCASE("Velocity with length zero does not lead to NaN") {
    const glm::vec2 velocity{0, 0};
    ALLOW_CALL(object, getVelocity()).RETURN(velocity);

    REQUIRE_CALL(object, addVelocityOffset(trompeloeil::_))
        .WITH(!glm::isnan(_1.x))
        .WITH(!glm::isnan(_1.y))
        .WITH(_1.x == doctest::Approx(0))
        .WITH(_1.y == doctest::Approx(0));

    Physics::Integrator{}.integrate(17ms, objects);
  }
}

TEST_CASE("Physics::Integrator checks for collisions (static non-moving objects)") {
  std::vector<std::unique_ptr<Physics::Object>> objects;
  objects.push_back(std::make_unique<MockObject>());
  objects.push_back(std::make_unique<MockObject>());

  auto &first_object = static_cast<MockObject &>(*objects.front());
  REQUIRE_CALL(first_object, update());

  auto &second_object = static_cast<MockObject &>(*objects.back());
  REQUIRE_CALL(second_object, update());

  const glm::vec2 no_velocity{0, 0};
  ALLOW_CALL(first_object, getVelocity()).RETURN(no_velocity);
  ALLOW_CALL(first_object, addVelocityOffset(trompeloeil::_));
  ALLOW_CALL(second_object, getVelocity()).RETURN(no_velocity);
  ALLOW_CALL(second_object, addVelocityOffset(trompeloeil::_));

  SUBCASE("No action if no collisions occur") {
    const ConvexBoundingPolygon empty_polygon{};
    ALLOW_CALL(first_object, getBoundingPolygon()).RETURN(empty_polygon);
    ALLOW_CALL(second_object, getBoundingPolygon()).RETURN(empty_polygon);
    Physics::Integrator{}.integrate(17ms, objects);
  }

  SUBCASE("Objects don't collide with themself") {
    const ConvexBoundingPolygon first_polygon{{0, 0}, {0, 1}, {1, 1}};
    ALLOW_CALL(first_object, getBoundingPolygon()).RETURN(first_polygon);

    const ConvexBoundingPolygon second_polygon{{5, 5}, {5, 6}, {6, 6}};
    ALLOW_CALL(second_object, getBoundingPolygon()).RETURN(second_polygon);

    Physics::Integrator{}.integrate(17ms, objects);
  }

  SUBCASE("Objects receive displacement vector on collision") {
    const ConvexBoundingPolygon first_quad{{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ALLOW_CALL(first_object, getBoundingPolygon()).RETURN(first_quad);

    const ConvexBoundingPolygon second_quad{{0.5, 0}, {0.5, 1}, {1.5, 1}, {1.5, 0}};
    ALLOW_CALL(second_object, getBoundingPolygon()).RETURN(second_quad);

    REQUIRE_CALL(first_object, handleCollisionWith(trompeloeil::_, trompeloeil::_))
        .LR_WITH(&_1 == &second_object)
        .WITH(_2.x == doctest::Approx(-0.5))
        .WITH(_2.y == doctest::Approx(0))
        .TIMES(2);
    REQUIRE_CALL(second_object, handleCollisionWith(trompeloeil::_, trompeloeil::_))
        .LR_WITH(&_1 == &first_object)
        .WITH(_2.x == doctest::Approx(0.5))
        .WITH(_2.y == doctest::Approx(0))
        .TIMES(2);

    Physics::Integrator{}.integrate(17ms, objects);
  }
}

TEST_CASE(
    "Physics::Integrator detects repeated collisions between moving objects during one tick") {
  std::vector<std::unique_ptr<Physics::Object>> objects;
  objects.push_back(std::make_unique<MockObject>());
  objects.push_back(std::make_unique<MockObject>());

  auto &first_object = static_cast<MockObject &>(*objects.front());
  ConvexBoundingPolygon first_polygon{{0, 0}, {0, 1}, {1, 1}, {1, 0}};
  const glm::vec2 first_velocity{0.4, 0};
  REQUIRE_CALL(first_object, update());
  ALLOW_CALL(first_object, getVelocity()).RETURN(first_velocity);
  ALLOW_CALL(first_object, addVelocityOffset(trompeloeil::_));
  ALLOW_CALL(first_object, getBoundingPolygon()).LR_RETURN(first_polygon);

  auto &second_object = static_cast<MockObject &>(*objects.back());
  const ConvexBoundingPolygon second_polygon{{0.5, 0}, {0.5, 1}, {1.5, 1}, {1.5, 0}};
  const glm::vec2 second_velocity{-0.3, 0};
  REQUIRE_CALL(second_object, update());
  ALLOW_CALL(second_object, getVelocity()).RETURN(second_velocity);
  ALLOW_CALL(second_object, addVelocityOffset(trompeloeil::_));
  ALLOW_CALL(second_object, getBoundingPolygon()).RETURN(second_polygon);

  SUBCASE("Objects move without reacting to collision") {
    REQUIRE_CALL(first_object, handleCollisionWith(trompeloeil::_, trompeloeil::_))
        .WITH(_2.x == doctest::Approx(-0.5))
        .TIMES(5);
    REQUIRE_CALL(second_object, handleCollisionWith(trompeloeil::_, trompeloeil::_))
        .WITH(_2.x == doctest::Approx(0.5))
        .TIMES(5);
    Physics::Integrator{}.integrate(17ms, objects);
  }

  SUBCASE("Object responds to collision by moving out of other object") {
    REQUIRE_CALL(first_object, handleCollisionWith(trompeloeil::_, trompeloeil::_))
        .WITH(_2.x == doctest::Approx(-0.5))
        .LR_SIDE_EFFECT(first_polygon.setPosition({200, 200}));
    REQUIRE_CALL(second_object, handleCollisionWith(trompeloeil::_, trompeloeil::_))
        .WITH(_2.x == doctest::Approx(0.5));
    Physics::Integrator{}.integrate(17ms, objects);
  }
}
