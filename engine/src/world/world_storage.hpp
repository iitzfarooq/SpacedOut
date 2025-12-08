#pragma once

#include <span>
#include <tuple>
#include <vector>
#include <memory>
#include <concepts>
#include <unordered_map>
#include "utils/types.hpp"
#include "models/base.hpp"
#include "models/artifact.hpp"
#include "models/wormhole.hpp"
#include "models/spaceship.hpp"
#include "models/celestial_bodies.hpp"

namespace ii {
    /**
     * WorldStorage: in-memory owner of world entities.
     * AF = stores exclusive ownership of Wormhole, Artifact, CelestialBody, and Spaceship
     *      and provides accessors + id lookup.
     * RI = each entity has id \ge 0; ownership is unique (std::unique_ptr).
     */
    class WorldStorage {
    private:
        std::vector<std::unique_ptr<Wormhole>> wormholes_;
        std::vector<std::unique_ptr<Artifact>> artifacts_;
        std::vector<std::unique_ptr<CelestialBody>> celestial_bodies_;
        std::vector<std::unique_ptr<Spaceship>> spaceships_;

        std::unordered_map<i32, BaseModel*> id_to_entity_;

    public:
        WorldStorage() = default;
        ~WorldStorage() = default;

        WorldStorage(const WorldStorage&) = delete;
        WorldStorage& operator=(const WorldStorage&) = delete;
        WorldStorage(WorldStorage&&) noexcept = default;
        WorldStorage& operator=(WorldStorage&&) noexcept = default;

        template<typename T, typename... Args>
        requires std::derived_from<T, BaseModel>
        Ref<T> add_entity(Args&&... args) {
            auto entity = std::make_unique<T>(std::forward<Args>(args)...);
            T* raw = entity.get();

            if constexpr (std::is_same_v<T, Wormhole>) {
                wormholes_.push_back(std::move(entity));
            } else if constexpr (std::is_same_v<T, Artifact>) {
                artifacts_.push_back(std::move(entity));
            } else if constexpr (std::derived_from<T, CelestialBody>) {
                celestial_bodies_.push_back(std::move(entity));
            } else if constexpr (std::is_same_v<T, Spaceship>) {
                spaceships_.push_back(std::move(entity));
            }

            id_to_entity_[raw->id] = raw;
            return std::ref(*raw);
        }

        auto wormholes() const -> std::span<const Uptr<Wormhole>>;
        auto artifacts() const -> std::span<const Uptr<Artifact>>;
        auto celestial_bodies() const -> std::span<const Uptr<CelestialBody>>;
        auto spaceships() const -> std::span<const Uptr<Spaceship>>;

        template <typename T>
        std::optional<CRef<T>> get_by_id(i32 id) const;

        template <typename T>
        std::optional<Ref<T>> get_by_id(i32 id);

        void clear();
        usize total_count() const;
        bool remove_by_id(i32 id);
    };
}
