#include <gtest/gtest.h>
#include "models/spaceship.hpp"

using namespace ii;

/**
 * Test suite partitions for Spaceship construction:
 * Inputs (preconditions):
 * - dry_mass: positive real (> 0)
 * - max_fuel: non-negative real (>= 0)
 * - exhaust_speed: non-negative real (>= 0)
 * - thrust_levels: non-empty vector of non-negative reals
 * Name arbitrary, id >= 0.
 * Expected:
 * - Members are set exactly; no mutation.
 */
TEST(SpaceshipTest, ConstructionSetsMembers) {
    i32 id = 5;
    std::string name{"falcon"};
    f64 dry_mass = 1000.0;
    f64 max_fuel = 500.0;
    f64 exhaust_speed = 2500.0;
    std::vector<f64> thrust_levels{0.0, 10.0, 20.5};

    Spaceship ship{id, name, dry_mass, max_fuel, thrust_levels, exhaust_speed};

    EXPECT_EQ(ship.id, id);
    EXPECT_EQ(ship.name, name);
    EXPECT_DOUBLE_EQ(ship.dry_mass, dry_mass);
    EXPECT_DOUBLE_EQ(ship.max_fuel, max_fuel);
    EXPECT_DOUBLE_EQ(ship.exhaust_speed, exhaust_speed);
    ASSERT_EQ(ship.thrust_levels.size(), thrust_levels.size());
    for (size_t i = 0; i < thrust_levels.size(); ++i) {
        EXPECT_DOUBLE_EQ(ship.thrust_levels[i], thrust_levels[i]);
    }
}

