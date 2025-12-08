#include <gtest/gtest.h>
#include "models/artifact.hpp"

using namespace ii;

/**
 * Test suite partitions for Artifact::pos(t):
 * Inputs:
 * - t partition: any real (no preconditions on time)
 * - position partition: arbitrary Vec2d
 * Preconditions:
 * - id >= 0 (BaseModel)
 * - name arbitrary
 * - position arbitrary
 * Expected:
 * - pos(t) == position for all t
 */
TEST(ArtifactTest, PosAlwaysReturnsFixedPosition) {
    Vec2d p{42.5, -13.0};
    Artifact a{0, std::string{"beacon"}, p};

    for (double t : {0.0, 1.0, 123.456, -10.0}) {
        EXPECT_DOUBLE_EQ(a.pos(t).x(), p.x());
        EXPECT_DOUBLE_EQ(a.pos(t).y(), p.y());
    }
}

TEST(ArtifactTest, ConstructionKeepsMembers) {
    Vec2d p{1.0, 2.0};
    std::string name{"node"};
    i32 id = 7;
    Artifact a{id, name, p};

    EXPECT_EQ(a.id, id);
    EXPECT_EQ(a.name, name);
    EXPECT_DOUBLE_EQ(a.position.x(), p.x());
    EXPECT_DOUBLE_EQ(a.position.y(), p.y());
}
