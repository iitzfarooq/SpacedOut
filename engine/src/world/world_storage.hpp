#pragma once

#include <vector>
#include <ranges>
#include <unordered_map>
#include <concepts>
#include "utils/types.hpp"
#include "models/base.hpp"
#include "models/artifact.hpp"
#include "models/wormhole.hpp"
#include "models/spaceship.hpp"
#include "models/celestial_bodies.hpp"

namespace ii {
    class WorldStorage {
    private:
        std::vector<std::unique_ptr<Wormhole>> wormholes_;
        std::vector<std::unique_ptr<Artifact>> artifacts_;
        std::vector<std::unique_ptr<CelestialBody>> celestial_bodies_;
        std::vector<std::unique_ptr<Spaceship>> spaceships_;

        std::unordered_map<i32, BaseModel*> id_to_entity_;

    public:
        WorldStorage() = default;

        template<typename T, typename... Args>
        requires std::derived_from<T, BaseModel>
        T* add_entity(Args&&... args) {
            auto entity = std::make_unique<T>(std::forward<Args>(args)...);
            T* raw = entity.get();

            if constexpr (std::is_same_v<T, Wormhole>) {
                wormholes_.push_back(std::move(entity));
            } else if constexpr (std::is_same_v<T, Artifact>) {
                artifacts_.push_back(std::move(entity));
            } else if constexpr (std::is_same_v<T, CelestialBody>) {
                celestial_bodies_.push_back(std::move(entity));
            } else if constexpr (std::is_same_v<T, Spaceship>) {
                spaceships_.push_back(std::move(entity));
            }

            id_to_entity_[raw->id] = raw;
            return raw;
        }

        inline auto& wormholes() { return wormholes_; }
        inline auto& artifacts() { return artifacts_; }
        inline auto& celestial_bodies() { return celestial_bodies_; }
        inline auto& spaceships() { return spaceships_; }

        inline const auto& wormholes() const { return wormholes_; }
        inline const auto& artifacts() const { return artifacts_; }
        inline const auto& celestial_bodies() const { return celestial_bodies_; }
        inline const auto& spaceships() const { return spaceships_; }

        template <typename T>
        inline T* get_by_id(i32 id) const {
            const auto it = id_to_entity_.find(id);
            return it != id_to_entity_.end() ? dynamic_cast<T*>(it->second) : nullptr;
        }

        inline void clear() {
            wormholes_.clear();
            artifacts_.clear();
            celestial_bodies_.clear();
            spaceships_.clear();
            id_to_entity_.clear();
        }

        inline usize total_count() const {
            return wormholes_.size() + artifacts_.size() + celestial_bodies_.size() + spaceships_.size();
        }

    };
}
