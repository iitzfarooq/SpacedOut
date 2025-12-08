#include <gtest/gtest.h>
#include "models/wormhole.hpp"

using namespace ii;

/**
 * Test suite partitions for Wormhole::is_open(t):
 * Inputs:
 * - t_open: non-negative real (pre: >= 0)
 * - t_close: real with t_close >= t_open (pre)
 * - t partition: any real
 * Expected:
 * - is_open(t) true for t in [t_open, t_close], false otherwise
 */
TEST(WormholeTest, IsOpenInclusiveRange) {
    Vec2d e{0.0, 0.0};
    Vec2d x{100.0, -50.0};
    Wormhole w{3, std::string{"gate"}, e, x, /*open*/ 10.0, /*close*/ 20.0};

    // Inside the window
    for (double t : {10.0, 15.0, 20.0}) {
        EXPECT_TRUE(w.is_open(t));
    }
    // Outside
    for (double t : {9.999, 0.0, -1.0, 20.001, 1e9}) {
        EXPECT_EQ(w.is_open(t), t >= 10.0 && t <= 20.0);
    }
}

TEST(WormholeTest, ConstructionKeepsMembers) {
    i32 id = 9;
    std::string name{"rendezvous"};
    Vec2d e{1.0, 2.0};
    Vec2d x{3.0, 4.0};
    f64 open = 0.0;
    f64 close = 100.0;

    Wormhole w{id, name, e, x, open, close};

    EXPECT_EQ(w.id, id);
    EXPECT_EQ(w.name, name);
    EXPECT_DOUBLE_EQ(w.entry.x(), e.x());
    EXPECT_DOUBLE_EQ(w.entry.y(), e.y());
    EXPECT_DOUBLE_EQ(w.exit.x(), x.x());
    EXPECT_DOUBLE_EQ(w.exit.y(), x.y());
    EXPECT_DOUBLE_EQ(w.t_open, open);
    EXPECT_DOUBLE_EQ(w.t_close, close);
}

