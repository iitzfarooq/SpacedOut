#include <gtest/gtest.h>
#include "models/celestial_bodies.hpp"
#include "physics/trajectory.hpp"

using namespace ii;

/**
 * Test suite partitions for CelestialBody with different Trajectories:
 * Inputs:
 * - radius partition: positive reals (pre: radius > 0)
 * - mass partition: positive reals (pre: mass > 0)
 * - trajectory partition: StationaryTrajectory, EllipticalTrajectory, StubTrajectory
 * - time partition: any real
 * Expected:
 * - StationaryTrajectory: pos(t) == position, vel(t) == (0,0)
 * - EllipticalTrajectory: correct orbital math (checked via stub or simple cases)
 * - Delegation: CelestialBody delegates to trajectory->pos(t) and trajectory->vel(t)
 */

TEST(CelestialBodiesTest, StationaryTrajectoryPosVel) {
    Vec2d p{10.0, -5.0};
    auto traj = std::make_unique<StationaryTrajectory>(p);
    CelestialBody body{1, std::string{"anchor"}, /*radius*/ 1.0, /*mass*/ 2.0, std::move(traj)};

    for (double t : {0.0, 2.5, -100.0, 1e6}) {
        auto pos = body.pos(t);
        auto vel = body.vel(t);
        EXPECT_DOUBLE_EQ(pos.x(), p.x());
        EXPECT_DOUBLE_EQ(pos.y(), p.y());
        EXPECT_DOUBLE_EQ(vel.x(), 0.0);
        EXPECT_DOUBLE_EQ(vel.y(), 0.0);
    }
}

// A simple stub trajectory for testing delegation behavior
struct StubTrajectory : public ii::EllipticalTrajectory {
    Vec2d pos_return{0.0, 0.0};
    Vec2d vel_return{0.0, 0.0};
    StubTrajectory(Vec2d p, Vec2d v) : EllipticalTrajectory(0,0,0,0,0,Vec2d{0,0}) {
        pos_return = p;
        vel_return = v;
    }
    virtual Vec2d pos(f64 /*t*/) const override { return pos_return; }
    virtual Vec2d vel(f64 /*t*/) const override { return vel_return; }
};

TEST(CelestialBodiesTest, CelestialBodyDelegatesToTrajectory) {
    auto traj = std::make_unique<StubTrajectory>(Vec2d{3.0, 4.0}, Vec2d{-1.0, 2.5});
    CelestialBody body{2, std::string{"orbiter"}, /*radius*/ 2.0, /*mass*/ 3.0, std::move(traj)};

    for (double t : {0.0, 10.0, -3.0}) {
        auto pos = body.pos(t);
        auto vel = body.vel(t);
        EXPECT_DOUBLE_EQ(pos.x(), 3.0);
        EXPECT_DOUBLE_EQ(pos.y(), 4.0);
        EXPECT_DOUBLE_EQ(vel.x(), -1.0);
        EXPECT_DOUBLE_EQ(vel.y(), 2.5);
    }
}

