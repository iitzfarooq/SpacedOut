/**
 * Test Suite: WorldStorage
 *
 * System under test: WorldStorage class
 * WorldStorage manages collections of entities (Wormhole, Artifact, CelestialBody, Spaceship)
 * with add, retrieve, remove, and query operations.
 *
 * ========== PARTITION DEFINITIONS ==========
 *
 * 1. add_entity<T>() partitions:
 *    - Entity type T: {Wormhole, Artifact, CelestialBody (OrbitalBody, StationaryBody), Spaceship}
 *    - Storage state: {empty, non-empty}
 *    - Number of entities added: {1, multiple}
 *
 * 2. get_by_id<T>() partitions:
 *    - ID existence: {exists, does not exist}
 *    - Type match: {correct type, wrong type}
 *    - Storage state: {empty, single entity, multiple entities}
 *    - Const vs non-const overload: {const, mutable}
 *
 * 3. remove_by_id() partitions:
 *    - ID existence: {exists, does not exist}
 *    - Entity type being removed: {Wormhole, Artifact, CelestialBody, Spaceship}
 *    - Storage state after removal: {becomes empty, still has entities}
 *
 * 4. clear() partitions:
 *    - Storage state before clear: {empty, single type populated, multiple types populated}
 *
 * 5. total_count() partitions:
 *    - Storage state: {empty, single entity, multiple entities of same type, mixed types}
 *
 * 6. Collection accessors (wormholes(), artifacts(), etc.) partitions:
 *    - Collection state: {empty, single element, multiple elements}
 *
 * ========== TEST COVERAGE MATRIX ==========
 * Each test covers at least one partition from each relevant dimension.
 */

#include <gtest/gtest.h>
#include "world/world_storage.hpp"
#include "models/wormhole.hpp"
#include "models/artifact.hpp"
#include "models/celestial_bodies.hpp"
#include "models/spaceship.hpp"

using namespace ii;

// ============================================================================
// Helper factory functions for creating valid test entities
// ============================================================================

namespace {
    Wormhole make_wormhole(i32 id, const std::string& name = "wormhole") {
        return Wormhole{id, name, Vec2d{0.0, 0.0}, Vec2d{100.0, 100.0}, 0.0, 100.0};
    }

    Artifact make_artifact(i32 id, const std::string& name = "artifact") {
        return Artifact{id, name, Vec2d{50.0, 50.0}};
    }

    StationaryBody make_stationary_body(i32 id, const std::string& name = "star") {
        return StationaryBody{id, name, 10.0, 1000.0, Vec2d{0.0, 0.0}};
    }

    Spaceship make_spaceship(i32 id, const std::string& name = "ship") {
        return Spaceship{id, name, 100.0, 50.0, {0.0, 10.0, 50.0}, 3000.0};
    }

    Uptr<SimpleOrbit> make_simple_orbit() {
        return std::make_unique<SimpleOrbit>(100.0, 80.0, 0.1, 0.0, 0.0, Vec2d{0.0, 0.0});
    }
}

// ============================================================================
// TEST SUITE: add_entity<T>()
// ============================================================================

/**
 * Partition coverage:
 * - Entity type: Wormhole
 * - Storage state: empty -> non-empty
 * - Number added: 1
 */
TEST(WorldStorageAddEntity, AddWormholeToEmptyStorage) {
    WorldStorage storage;

    auto ref = storage.add_entity<Wormhole>(
        1, std::string{"gate-alpha"},
        Vec2d{0.0, 0.0}, Vec2d{100.0, 200.0},
        10.0, 50.0
    );

    EXPECT_EQ(ref.get().id, 1);
    EXPECT_EQ(ref.get().name, "gate-alpha");
    EXPECT_EQ(storage.total_count(), 1);
    EXPECT_EQ(storage.wormholes().size(), 1);
}

/**
 * Partition coverage:
 * - Entity type: Artifact
 * - Storage state: empty -> non-empty
 * - Number added: 1
 */
TEST(WorldStorageAddEntity, AddArtifactToEmptyStorage) {
    WorldStorage storage;

    auto ref = storage.add_entity<Artifact>(
        2, std::string{"relic"},
        Vec2d{25.0, 75.0}
    );

    EXPECT_EQ(ref.get().id, 2);
    EXPECT_EQ(ref.get().name, "relic");
    EXPECT_EQ(storage.artifacts().size(), 1);
}

/**
 * Partition coverage:
 * - Entity type: StationaryBody (CelestialBody subtype)
 * - Storage state: empty -> non-empty
 * - Number added: 1
 */
TEST(WorldStorageAddEntity, AddStationaryBodyToEmptyStorage) {
    WorldStorage storage;

    auto ref = storage.add_entity<StationaryBody>(
        3, std::string{"sun"},
        695700.0, 1.989e30,
        Vec2d{0.0, 0.0}
    );

    EXPECT_EQ(ref.get().id, 3);
    EXPECT_EQ(ref.get().name, "sun");
    EXPECT_EQ(storage.celestial_bodies().size(), 1);
}

/**
 * Partition coverage:
 * - Entity type: OrbitalBody (CelestialBody subtype)
 * - Storage state: empty -> non-empty
 * - Number added: 1
 */
TEST(WorldStorageAddEntity, AddOrbitalBodyToEmptyStorage) {
    WorldStorage storage;

    auto ref = storage.add_entity<OrbitalBody>(
        4, std::string{"planet"},
        6371.0, 5.972e24,
        make_simple_orbit()
    );

    EXPECT_EQ(ref.get().id, 4);
    EXPECT_EQ(ref.get().name, "planet");
    EXPECT_EQ(storage.celestial_bodies().size(), 1);
}

/**
 * Partition coverage:
 * - Entity type: Spaceship
 * - Storage state: empty -> non-empty
 * - Number added: 1
 */
TEST(WorldStorageAddEntity, AddSpaceshipToEmptyStorage) {
    WorldStorage storage;

    auto ref = storage.add_entity<Spaceship>(
        5, std::string{"explorer"},
        1000.0, 500.0,
        std::vector<f64>{0.0, 100.0, 500.0},
        3000.0
    );

    EXPECT_EQ(ref.get().id, 5);
    EXPECT_EQ(ref.get().name, "explorer");
    EXPECT_EQ(storage.spaceships().size(), 1);
}

/**
 * Partition coverage:
 * - Entity types: Multiple different types
 * - Storage state: non-empty (progressively filling)
 * - Number added: multiple
 */
TEST(WorldStorageAddEntity, AddMultipleEntitiesOfDifferentTypes) {
    WorldStorage storage;

    storage.add_entity<Wormhole>(1, std::string{"wh1"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Wormhole>(2, std::string{"wh2"}, Vec2d{5,5}, Vec2d{15,15}, 5.0, 15.0);
    storage.add_entity<Artifact>(3, std::string{"art1"}, Vec2d{20,20});
    storage.add_entity<StationaryBody>(4, std::string{"star"}, 100.0, 1e10, Vec2d{0,0});
    storage.add_entity<Spaceship>(5, std::string{"ship1"}, 100.0, 50.0, std::vector<f64>{0.0, 10.0}, 3000.0);

    EXPECT_EQ(storage.wormholes().size(), 2);
    EXPECT_EQ(storage.artifacts().size(), 1);
    EXPECT_EQ(storage.celestial_bodies().size(), 1);
    EXPECT_EQ(storage.spaceships().size(), 1);
    EXPECT_EQ(storage.total_count(), 5);
}

/**
 * Partition coverage:
 * - Entity type: same type multiple times
 * - Storage state: non-empty -> more entities
 * - Number added: multiple of same type
 */
TEST(WorldStorageAddEntity, AddMultipleEntitiesOfSameType) {
    WorldStorage storage;

    for (i32 i = 0; i < 5; ++i) {
        storage.add_entity<Artifact>(i, "artifact_" + std::to_string(i), Vec2d{i * 10.0, i * 10.0});
    }

    EXPECT_EQ(storage.artifacts().size(), 5);
    EXPECT_EQ(storage.total_count(), 5);
}

// ============================================================================
// TEST SUITE: get_by_id<T>() (const and mutable)
// ============================================================================

/**
 * Partition coverage:
 * - ID existence: exists
 * - Type match: correct type
 * - Storage state: single entity
 * - Accessor: const
 */
TEST(WorldStorageGetById, GetExistingEntityByIdConst) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(42, std::string{"portal"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 100.0);

    const WorldStorage& const_storage = storage;
    auto result = const_storage.get_by_id<Wormhole>(42);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->get().id, 42);
    EXPECT_EQ(result->get().name, "portal");
}

/**
 * Partition coverage:
 * - ID existence: exists
 * - Type match: correct type
 * - Storage state: single entity
 * - Accessor: mutable
 */
TEST(WorldStorageGetById, GetExistingEntityByIdMutable) {
    WorldStorage storage;
    storage.add_entity<Artifact>(7, std::string{"gem"}, Vec2d{1.0, 2.0});

    auto result = storage.get_by_id<Artifact>(7);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->get().id, 7);
    EXPECT_EQ(result->get().name, "gem");
}

/**
 * Partition coverage:
 * - ID existence: does not exist
 * - Storage state: empty
 */
TEST(WorldStorageGetById, GetNonExistingIdFromEmptyStorage) {
    WorldStorage storage;
    const WorldStorage& const_storage = storage;

    auto result = const_storage.get_by_id<Wormhole>(999);

    EXPECT_FALSE(result.has_value());
}

/**
 * Partition coverage:
 * - ID existence: does not exist
 * - Storage state: non-empty (has other entities)
 */
TEST(WorldStorageGetById, GetNonExistingIdFromNonEmptyStorage) {
    WorldStorage storage;
    storage.add_entity<Artifact>(1, std::string{"item"}, Vec2d{0,0});
    storage.add_entity<Wormhole>(2, std::string{"gate"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);

    const WorldStorage& const_storage = storage;
    auto result = const_storage.get_by_id<Artifact>(999);

    EXPECT_FALSE(result.has_value());
}

/**
 * Partition coverage:
 * - ID existence: exists
 * - Type match: wrong type (ID exists but as different type)
 * - Storage state: multiple entities
 */
TEST(WorldStorageGetById, GetExistingIdWithWrongType) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(10, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);

    const WorldStorage& const_storage = storage;
    // ID 10 exists as Wormhole, but we request as Artifact
    auto result = const_storage.get_by_id<Artifact>(10);

    EXPECT_FALSE(result.has_value());
}

/**
 * Partition coverage:
 * - ID existence: exists
 * - Type match: correct type
 * - Storage state: multiple entities of mixed types
 */
TEST(WorldStorageGetById, GetCorrectEntityFromMixedStorage) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(1, std::string{"wh1"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Artifact>(2, std::string{"art1"}, Vec2d{5,5});
    storage.add_entity<StationaryBody>(3, std::string{"star"}, 100.0, 1e10, Vec2d{0,0});
    storage.add_entity<Spaceship>(4, std::string{"ship"}, 100.0, 50.0, std::vector<f64>{0.0}, 3000.0);

    const WorldStorage& cs = storage;

    auto wh = cs.get_by_id<Wormhole>(1);
    auto art = cs.get_by_id<Artifact>(2);
    auto cb = cs.get_by_id<CelestialBody>(3);
    auto ship = cs.get_by_id<Spaceship>(4);

    ASSERT_TRUE(wh.has_value());
    ASSERT_TRUE(art.has_value());
    ASSERT_TRUE(cb.has_value());
    ASSERT_TRUE(ship.has_value());

    EXPECT_EQ(wh->get().name, "wh1");
    EXPECT_EQ(art->get().name, "art1");
    EXPECT_EQ(cb->get().name, "star");
    EXPECT_EQ(ship->get().name, "ship");
}

// ============================================================================
// TEST SUITE: remove_by_id()
// ============================================================================

/**
 * Partition coverage:
 * - ID existence: exists
 * - Entity type: Wormhole
 * - Storage state after: becomes empty
 */
TEST(WorldStorageRemove, RemoveOnlyWormhole) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(1, std::string{"gate"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);

    EXPECT_EQ(storage.total_count(), 1);

    bool removed = storage.remove_by_id(1);

    EXPECT_TRUE(removed);
    EXPECT_EQ(storage.total_count(), 0);
    EXPECT_EQ(storage.wormholes().size(), 0);
}

/**
 * Partition coverage:
 * - ID existence: exists
 * - Entity type: Artifact
 * - Storage state after: still has other entities
 */
TEST(WorldStorageRemove, RemoveArtifactFromMixedStorage) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(1, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Artifact>(2, std::string{"art"}, Vec2d{5,5});
    storage.add_entity<StationaryBody>(3, std::string{"star"}, 100.0, 1e10, Vec2d{0,0});

    EXPECT_EQ(storage.total_count(), 3);

    bool removed = storage.remove_by_id(2);

    EXPECT_TRUE(removed);
    EXPECT_EQ(storage.total_count(), 2);
    EXPECT_EQ(storage.artifacts().size(), 0);
    EXPECT_EQ(storage.wormholes().size(), 1);
    EXPECT_EQ(storage.celestial_bodies().size(), 1);
}

/**
 * Partition coverage:
 * - ID existence: does not exist
 * - Storage state: empty
 */
TEST(WorldStorageRemove, RemoveFromEmptyStorage) {
    WorldStorage storage;

    bool removed = storage.remove_by_id(999);

    EXPECT_FALSE(removed);
    EXPECT_EQ(storage.total_count(), 0);
}

/**
 * Partition coverage:
 * - ID existence: does not exist
 * - Storage state: non-empty
 */
TEST(WorldStorageRemove, RemoveNonExistingIdFromNonEmptyStorage) {
    WorldStorage storage;
    storage.add_entity<Artifact>(1, std::string{"item"}, Vec2d{0,0});

    bool removed = storage.remove_by_id(999);

    EXPECT_FALSE(removed);
    EXPECT_EQ(storage.total_count(), 1);
}

/**
 * Partition coverage:
 * - ID existence: exists
 * - Entity type: CelestialBody (StationaryBody)
 * - Storage state after: still has entities
 */
TEST(WorldStorageRemove, RemoveCelestialBody) {
    WorldStorage storage;
    storage.add_entity<StationaryBody>(1, std::string{"sun"}, 100.0, 1e10, Vec2d{0,0});
    storage.add_entity<OrbitalBody>(2, std::string{"earth"}, 50.0, 1e8, make_simple_orbit());

    EXPECT_EQ(storage.celestial_bodies().size(), 2);

    bool removed = storage.remove_by_id(1);

    EXPECT_TRUE(removed);
    EXPECT_EQ(storage.celestial_bodies().size(), 1);
}

/**
 * Partition coverage:
 * - ID existence: exists
 * - Entity type: Spaceship
 * - Storage state after: becomes empty
 */
TEST(WorldStorageRemove, RemoveOnlySpaceship) {
    WorldStorage storage;
    storage.add_entity<Spaceship>(1, std::string{"ship"}, 100.0, 50.0, std::vector<f64>{0.0, 10.0}, 3000.0);

    bool removed = storage.remove_by_id(1);

    EXPECT_TRUE(removed);
    EXPECT_EQ(storage.spaceships().size(), 0);
    EXPECT_EQ(storage.total_count(), 0);
}

/**
 * Partition coverage:
 * - Sequential removal of all entities
 * - Verifies ID lookup is updated correctly after each removal
 */
TEST(WorldStorageRemove, RemoveAllEntitiesSequentially) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(1, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Artifact>(2, std::string{"art"}, Vec2d{5,5});
    storage.add_entity<StationaryBody>(3, std::string{"star"}, 100.0, 1e10, Vec2d{0,0});

    EXPECT_EQ(storage.total_count(), 3);

    EXPECT_TRUE(storage.remove_by_id(1));
    EXPECT_EQ(storage.total_count(), 2);

    EXPECT_TRUE(storage.remove_by_id(2));
    EXPECT_EQ(storage.total_count(), 1);

    EXPECT_TRUE(storage.remove_by_id(3));
    EXPECT_EQ(storage.total_count(), 0);

    // Verify cannot remove already removed items
    EXPECT_FALSE(storage.remove_by_id(1));
    EXPECT_FALSE(storage.remove_by_id(2));
    EXPECT_FALSE(storage.remove_by_id(3));
}

// ============================================================================
// TEST SUITE: clear()
// ============================================================================

/**
 * Partition coverage:
 * - Storage state before: empty
 */
TEST(WorldStorageClear, ClearEmptyStorage) {
    WorldStorage storage;

    storage.clear();

    EXPECT_EQ(storage.total_count(), 0);
}

/**
 * Partition coverage:
 * - Storage state before: single type populated
 */
TEST(WorldStorageClear, ClearStorageWithSingleType) {
    WorldStorage storage;
    storage.add_entity<Artifact>(1, std::string{"art1"}, Vec2d{0,0});
    storage.add_entity<Artifact>(2, std::string{"art2"}, Vec2d{10,10});

    EXPECT_EQ(storage.total_count(), 2);

    storage.clear();

    EXPECT_EQ(storage.total_count(), 0);
    EXPECT_EQ(storage.artifacts().size(), 0);
}

/**
 * Partition coverage:
 * - Storage state before: multiple types populated
 */
TEST(WorldStorageClear, ClearStorageWithMultipleTypes) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(1, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Artifact>(2, std::string{"art"}, Vec2d{5,5});
    storage.add_entity<StationaryBody>(3, std::string{"star"}, 100.0, 1e10, Vec2d{0,0});
    storage.add_entity<Spaceship>(4, std::string{"ship"}, 100.0, 50.0, std::vector<f64>{0.0}, 3000.0);

    EXPECT_EQ(storage.total_count(), 4);

    storage.clear();

    EXPECT_EQ(storage.total_count(), 0);
    EXPECT_EQ(storage.wormholes().size(), 0);
    EXPECT_EQ(storage.artifacts().size(), 0);
    EXPECT_EQ(storage.celestial_bodies().size(), 0);
    EXPECT_EQ(storage.spaceships().size(), 0);
}

/**
 * Partition coverage:
 * - Verify clear invalidates ID lookups
 */
TEST(WorldStorageClear, ClearInvalidatesIdLookups) {
    WorldStorage storage;
    storage.add_entity<Artifact>(1, std::string{"art"}, Vec2d{0,0});

    const WorldStorage& cs = storage;
    ASSERT_TRUE(cs.get_by_id<Artifact>(1).has_value());

    storage.clear();

    EXPECT_FALSE(cs.get_by_id<Artifact>(1).has_value());
}

// ============================================================================
// TEST SUITE: total_count()
// ============================================================================

/**
 * Partition coverage:
 * - Storage state: empty
 */
TEST(WorldStorageTotalCount, EmptyStorageHasZeroCount) {
    WorldStorage storage;

    EXPECT_EQ(storage.total_count(), 0);
}

/**
 * Partition coverage:
 * - Storage state: single entity
 */
TEST(WorldStorageTotalCount, SingleEntityCount) {
    WorldStorage storage;
    storage.add_entity<Artifact>(1, std::string{"art"}, Vec2d{0,0});

    EXPECT_EQ(storage.total_count(), 1);
}

/**
 * Partition coverage:
 * - Storage state: multiple entities of same type
 */
TEST(WorldStorageTotalCount, MultipleEntitiesSameType) {
    WorldStorage storage;
    for (i32 i = 0; i < 10; ++i) {
        storage.add_entity<Wormhole>(i, "wh_" + std::to_string(i), Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    }

    EXPECT_EQ(storage.total_count(), 10);
}

/**
 * Partition coverage:
 * - Storage state: mixed types
 */
TEST(WorldStorageTotalCount, MixedTypesCount) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(1, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Wormhole>(2, std::string{"wh2"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Artifact>(3, std::string{"art"}, Vec2d{5,5});
    storage.add_entity<StationaryBody>(4, std::string{"star"}, 100.0, 1e10, Vec2d{0,0});
    storage.add_entity<Spaceship>(5, std::string{"ship"}, 100.0, 50.0, std::vector<f64>{0.0}, 3000.0);
    storage.add_entity<Spaceship>(6, std::string{"ship2"}, 200.0, 100.0, std::vector<f64>{0.0, 50.0}, 3500.0);

    EXPECT_EQ(storage.total_count(), 6);
}

// ============================================================================
// TEST SUITE: Collection accessors
// ============================================================================

/**
 * Partition coverage:
 * - Collection state: empty
 */
TEST(WorldStorageAccessors, EmptyCollections) {
    WorldStorage storage;

    EXPECT_EQ(storage.wormholes().size(), 0);
    EXPECT_EQ(storage.artifacts().size(), 0);
    EXPECT_EQ(storage.celestial_bodies().size(), 0);
    EXPECT_EQ(storage.spaceships().size(), 0);
}

/**
 * Partition coverage:
 * - Collection state: single element each
 */
TEST(WorldStorageAccessors, SingleElementCollections) {
    WorldStorage storage;
    storage.add_entity<Wormhole>(1, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);
    storage.add_entity<Artifact>(2, std::string{"art"}, Vec2d{5,5});
    storage.add_entity<StationaryBody>(3, std::string{"star"}, 100.0, 1e10, Vec2d{0,0});
    storage.add_entity<Spaceship>(4, std::string{"ship"}, 100.0, 50.0, std::vector<f64>{0.0}, 3000.0);

    auto wormholes = storage.wormholes();
    auto artifacts = storage.artifacts();
    auto celestial_bodies = storage.celestial_bodies();
    auto spaceships = storage.spaceships();

    ASSERT_EQ(wormholes.size(), 1);
    ASSERT_EQ(artifacts.size(), 1);
    ASSERT_EQ(celestial_bodies.size(), 1);
    ASSERT_EQ(spaceships.size(), 1);

    EXPECT_EQ(wormholes[0]->id, 1);
    EXPECT_EQ(artifacts[0]->id, 2);
    EXPECT_EQ(celestial_bodies[0]->id, 3);
    EXPECT_EQ(spaceships[0]->id, 4);
}

/**
 * Partition coverage:
 * - Collection state: multiple elements
 * - Verify ordering (insertion order preserved)
 */
TEST(WorldStorageAccessors, MultipleElementsPreserveOrder) {
    WorldStorage storage;
    storage.add_entity<Artifact>(1, std::string{"first"}, Vec2d{0,0});
    storage.add_entity<Artifact>(2, std::string{"second"}, Vec2d{10,10});
    storage.add_entity<Artifact>(3, std::string{"third"}, Vec2d{20,20});

    auto artifacts = storage.artifacts();

    ASSERT_EQ(artifacts.size(), 3);
    EXPECT_EQ(artifacts[0]->name, "first");
    EXPECT_EQ(artifacts[1]->name, "second");
    EXPECT_EQ(artifacts[2]->name, "third");
}

// ============================================================================
// TEST SUITE: Move semantics
// ============================================================================

/**
 * Partition coverage:
 * - Move construction transfers ownership
 */
TEST(WorldStorageMove, MoveConstructionTransfersOwnership) {
    WorldStorage storage1;
    storage1.add_entity<Artifact>(1, std::string{"art"}, Vec2d{0,0});
    storage1.add_entity<Wormhole>(2, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);

    WorldStorage storage2 = std::move(storage1);

    EXPECT_EQ(storage2.total_count(), 2);
    EXPECT_EQ(storage2.artifacts().size(), 1);
    EXPECT_EQ(storage2.wormholes().size(), 1);
}

/**
 * Partition coverage:
 * - Move assignment transfers ownership
 */
TEST(WorldStorageMove, MoveAssignmentTransfersOwnership) {
    WorldStorage storage1;
    storage1.add_entity<Artifact>(1, std::string{"art"}, Vec2d{0,0});

    WorldStorage storage2;
    storage2.add_entity<Wormhole>(2, std::string{"wh"}, Vec2d{0,0}, Vec2d{10,10}, 0.0, 10.0);

    storage2 = std::move(storage1);

    EXPECT_EQ(storage2.total_count(), 1);
    EXPECT_EQ(storage2.artifacts().size(), 1);
    EXPECT_EQ(storage2.wormholes().size(), 0);
}

