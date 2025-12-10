#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include "world/world_query.hpp"
#include "world/world_storage.hpp"
#include "models/artifact.hpp"
#include "models/celestial_bodies.hpp"
#include "models/wormhole.hpp"
#include "models/spaceship.hpp"
#include "world/trajectory.hpp"

using namespace ii;

// Helper to count range elements
template<typename R>
size_t count_range(R&& range) {
    return std::ranges::distance(range);
}

class WorldQueryTest : public ::testing::Test {
protected:
    WorldStorage storage;
    
    void SetUp() override {
        // 1. Artifacts
        // A1: (10, 0)
        storage.add_entity<Artifact>(1, "A1", Vec2d{10.0, 0.0});
        // A2: (-5, 5)
        storage.add_entity<Artifact>(2, "A2", Vec2d{-5.0, 5.0});

        // 2. Stationary Bodies (CelestialBody with StationaryTrajectory)
        // S1: (0, 0), Mass 100
        auto traj1 = std::make_unique<StationaryTrajectory>(Vec2d{0.0, 0.0});
        storage.add_entity<CelestialBody>(3, "Sun", 10.0, 100.0, std::move(traj1));
        
        // S2: (20, 20), Mass 50
        auto traj2 = std::make_unique<StationaryTrajectory>(Vec2d{20.0, 20.0});
        storage.add_entity<CelestialBody>(4, "Star2", 5.0, 50.0, std::move(traj2));

        // 3. Orbital Bodies (CelestialBody with EllipticalTrajectory)
        // O1: Orbit around (0,0), radius 10, period 2pi (w=1)
        // pos(t) = (10 cos(t), 10 sin(t))
        auto traj3 = std::make_unique<EllipticalTrajectory>(10.0, 10.0, 1.0, 0.0, 0.0, Vec2d{0.0, 0.0});
        storage.add_entity<CelestialBody>(5, "Planet1", 2.0, 10.0, std::move(traj3));

        // 4. Wormholes
        // W1: Open [0, 10]
        storage.add_entity<Wormhole>(6, "W1", Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
        // W2: Open [20, 30]
        storage.add_entity<Wormhole>(7, "W2", Vec2d{5,5}, Vec2d{15,15}, 20.0, 30.0);

        // 5. Spaceships
        std::vector<f64> thrust = {1.0, 2.0};
        storage.add_entity<Spaceship>(8, "Ship1", 100.0, 50.0, thrust, 10.0);
    }
};

/**
 * Test Suite: WorldQuery Basic Retrieval
 * Partitions:
 * - Entity Types: Wormhole, Artifact, CelestialBody, Spaceship
 * - Storage State: Populated
 */
TEST_F(WorldQueryTest, RetrieveAllEntitiesByType) {
    WorldQuery query(storage);

    // Wormholes
    auto whs = query.all_wormholes();
    EXPECT_EQ(count_range(whs), 2);

    // Artifacts
    auto arts = query.all_artifacts();
    EXPECT_EQ(count_range(arts), 2);

    // Celestial Bodies (Stationary + Orbital)
    auto cbs = query.all_celestial_bodies();
    EXPECT_EQ(count_range(cbs), 3); // 2 Stationary + 1 Orbital

    // Spaceships
    auto ships = query.all_spaceships();
    EXPECT_EQ(count_range(ships), 1);
}

/**
 * Test Suite: WorldQuery::within_radius
 * Partitions:
 * - Center: Origin, Offset
 * - Radius: Small (subset), Large (all), Zero
 * - Time: t=0, t>0 (affects OrbitalBody)
 * - Boundary: Exact match
 */
TEST_F(WorldQueryTest, WithinRadius_Artifacts) {
    WorldQuery query(storage);
    
    // Center (0,0), Radius 11 -> Should include A1(10,0) and A2(-5,5) [dist ~7.07]
    auto res1 = query.within_radius<Artifact>(Vec2d{0,0}, 11.0, 0.0);
    EXPECT_EQ(count_range(res1), 2);

    // Center (0,0), Radius 8 -> Should include A2(-5,5) only
    auto res2 = query.within_radius<Artifact>(Vec2d{0,0}, 8.0, 0.0);
    EXPECT_EQ(count_range(res2), 1);
    EXPECT_EQ((*res2.begin()).get().name, "A2");

    // Center (10,0), Radius 0 -> Should include A1(10,0)
    auto res3 = query.within_radius<Artifact>(Vec2d{10,0}, 0.0, 0.0);
    EXPECT_EQ(count_range(res3), 1);
    EXPECT_EQ((*res3.begin()).get().name, "A1");
}

TEST_F(WorldQueryTest, WithinRadius_CelestialBodies_TimeDependence) {
    WorldQuery query(storage);

    // O1 at t=0 is at (10, 0)
    // S1 is at (0, 0)
    // S2 is at (20, 20)

    // t=0, Center (0,0), Radius 10.0
    // Should include S1 (dist 0) and O1 (dist 10)
    auto res_t0 = query.within_radius<CelestialBody>(Vec2d{0,0}, 10.0, 0.0);
    EXPECT_EQ(count_range(res_t0), 2); 

    // t=pi (approx 3.14159), O1 should be at (-10, 0)
    // Center (10,0), Radius 1.0
    // At t=0, O1 is at (10,0) -> Inside
    // At t=pi, O1 is at (-10,0) -> Outside
    
    auto res_t0_loc = query.within_radius<CelestialBody>(Vec2d{10,0}, 1.0, 0.0);
    // Should find O1
    bool found_t0 = false;
    for(const auto& cb : res_t0_loc) {
        if(cb.get().name == "Planet1") found_t0 = true;
    }
    EXPECT_TRUE(found_t0);

    auto res_tpi_loc = query.within_radius<CelestialBody>(Vec2d{10,0}, 1.0, std::numbers::pi);
    // Should NOT find O1
    bool found_tpi = false;
    for(const auto& cb : res_tpi_loc) {
        if(cb.get().name == "Planet1") found_tpi = true;
    }
    EXPECT_FALSE(found_tpi);
}

/**
 * Test Suite: WorldQuery::within_rect
 * Partitions:
 * - Rect: Enclosing, Disjoint, Partial
 * - Boundary: On edge
 */
TEST_F(WorldQueryTest, WithinRect) {
    WorldQuery query(storage);

    // A1: (10, 0), A2: (-5, 5)

    // Rect [-10, -10] to [15, 15] -> Should include both
    auto res1 = query.within_rect<Artifact>(Vec2d{-10,-10}, Vec2d{15,15}, 0.0);
    EXPECT_EQ(count_range(res1), 2);

    // Rect [0, 0] to [12, 12] -> Should include A1(10,0) only
    // A2 is at x=-5, outside
    auto res2 = query.within_rect<Artifact>(Vec2d{0,0}, Vec2d{12,12}, 0.0);
    EXPECT_EQ(count_range(res2), 1);
    EXPECT_EQ((*res2.begin()).get().name, "A1");

    // Boundary check: Rect [10, 0] to [20, 10]
    // A1 is exactly on min corner
    auto res3 = query.within_rect<Artifact>(Vec2d{10,0}, Vec2d{20,10}, 0.0);
    EXPECT_EQ(count_range(res3), 1);
}

/**
 * Test Suite: WorldQuery::open_wormholes
 * Partitions:
 * - Time: Before open, At open, Inside, At close, After close
 */
TEST_F(WorldQueryTest, OpenWormholes) {
    WorldQuery query(storage);
    // W1: [0, 10]
    // W2: [20, 30]

    // t = -1: None
    EXPECT_EQ(count_range(query.open_wormholes(-1.0)), 0);

    // t = 0: W1 open (boundary start)
    auto res_t0 = query.open_wormholes(0.0);
    EXPECT_EQ(count_range(res_t0), 1);
    EXPECT_EQ((*res_t0.begin()).get().name, "W1");

    // t = 5: W1 open
    EXPECT_EQ(count_range(query.open_wormholes(5.0)), 1);

    // t = 10: W1 open (boundary end)
    EXPECT_EQ(count_range(query.open_wormholes(10.0)), 1);

    // t = 15: None
    EXPECT_EQ(count_range(query.open_wormholes(15.0)), 0);

    // t = 25: W2 open
    auto res_t25 = query.open_wormholes(25.0);
    EXPECT_EQ(count_range(res_t25), 1);
    EXPECT_EQ((*res_t25.begin()).get().name, "W2");
}

/**
 * Test Suite: WorldQuery::massive_bodies
 * Partitions:
 * - Mass threshold: < min, = min, > min
 */
TEST_F(WorldQueryTest, MassiveBodies) {
    WorldQuery query(storage);
    // S1: Mass 100
    // S2: Mass 50
    // O1: Mass 10

    // Min mass 110 -> None
    EXPECT_EQ(count_range(query.massive_bodies(110.0)), 0);

    // Min mass 100 -> S1
    auto res1 = query.massive_bodies(100.0);
    EXPECT_EQ(count_range(res1), 1);
    EXPECT_EQ((*res1.begin()).get().name, "Sun");

    // Min mass 50 -> S1, S2
    auto res2 = query.massive_bodies(50.0);
    EXPECT_EQ(count_range(res2), 2);

    // Min mass 10 -> S1, S2, O1
    EXPECT_EQ(count_range(query.massive_bodies(10.0)), 3);
    
    // Min mass 0 -> All
    EXPECT_EQ(count_range(query.massive_bodies(0.0)), 3);
}

/**
 * Test Suite: WorldQuery::orbital_bodies
 * Partitions:
 * - Type check: Should only return CelestialBody with EllipticalTrajectory (not StationaryTrajectory)
 */
TEST_F(WorldQueryTest, OrbitalBodiesOnly) {
    WorldQuery query(storage);
    // S1, S2 are Stationary
    // O1 is Orbital

    auto res = query.orbital_bodies();
    EXPECT_EQ(count_range(res), 1);
    EXPECT_EQ((*res.begin()).get().name, "Planet1");
}

/**
 * Test Suite: WorldQuery::stationary_bodies
 * Partitions:
 * - Type check: Should only return CelestialBody with StationaryTrajectory
 */
TEST_F(WorldQueryTest, StationaryBodiesOnly) {
    WorldQuery query(storage);
    // S1, S2 are Stationary
    // O1 is Orbital

    auto res = query.stationary_bodies();
    EXPECT_EQ(count_range(res), 2);
}

/**
 * Test Suite: WorldQuery::within_rect with Moving Bodies
 */
TEST_F(WorldQueryTest, WithinRect_MovingBodies) {
    WorldQuery query(storage);

    // O1: Orbit radius 10 around (0,0).
    // t=0: (10, 0)
    // t=pi/2: (0, 10)
    // t=pi: (-10, 0)
    // t=3pi/2: (0, -10)

    // Rect [5, -5] to [15, 5] (Contains (10,0))
    
    // t=0: O1 is at (10,0) -> Inside
    auto res_t0 = query.within_rect<CelestialBody>(Vec2d{5,-5}, Vec2d{15,5}, 0.0);
    bool found_t0 = false;
    for(const auto& cb : res_t0) {
        if(cb.get().name == "Planet1") found_t0 = true;
    }
    EXPECT_TRUE(found_t0);

    // t=pi: O1 is at (-10,0) -> Outside
    auto res_tpi = query.within_rect<CelestialBody>(Vec2d{5,-5}, Vec2d{15,5}, std::numbers::pi);
    bool found_tpi = false;
    for(const auto& cb : res_tpi) {
        if(cb.get().name == "Planet1") found_tpi = true;
    }
    EXPECT_FALSE(found_tpi);
}

/**
 * Test Suite: Empty Storage
 */
TEST(WorldQueryEmptyTest, EmptyStorageReturnsEmptyRanges) {
    WorldStorage empty_storage;
    WorldQuery query(empty_storage);

    EXPECT_EQ(count_range(query.all_wormholes()), 0);
    EXPECT_EQ(count_range(query.all_artifacts()), 0);
    EXPECT_EQ(count_range(query.all_celestial_bodies()), 0);
    EXPECT_EQ(count_range(query.all_spaceships()), 0);
    
    EXPECT_EQ(count_range(query.within_radius<Artifact>(Vec2d{0,0}, 100.0, 0.0)), 0);
    EXPECT_EQ(count_range(query.open_wormholes(0.0)), 0);
}

