#include "world_storage.hpp"

namespace ii {
    auto WorldStorage::wormholes() const -> std::span<const Uptr<Wormhole>> {
        return {wormholes_.data(), wormholes_.size()};
    }

    auto WorldStorage::artifacts() const -> std::span<const Uptr<Artifact>> {
        return {artifacts_.data(), artifacts_.size()};
    }

    auto WorldStorage::celestial_bodies() const -> std::span<const Uptr<CelestialBody>> {
        return {celestial_bodies_.data(), celestial_bodies_.size()};
    }

    auto WorldStorage::spaceships() const -> std::span<const Uptr<Spaceship>> {
        return {spaceships_.data(), spaceships_.size()};
    }

    template <typename T>
    std::optional<CRef<T>> WorldStorage::get_by_id(i32 id) const {
        const auto it = id_to_entity_.find(id);
        if (it != id_to_entity_.end()) {
            if (auto* casted = dynamic_cast<const T*>(it->second)) {
                return std::cref(*casted);
            }
        }
        return std::nullopt;
    }

    template <typename T>
    std::optional<Ref<T>> WorldStorage::get_by_id(i32 id) {
        const auto it = id_to_entity_.find(id);
        if (it != id_to_entity_.end()) {
            if (auto* casted = dynamic_cast<T*>(it->second)) {
                return std::ref(*casted);
            }
        }
        return std::nullopt;
    }

    void WorldStorage::clear() {
        wormholes_.clear();
        artifacts_.clear();
        celestial_bodies_.clear();
        spaceships_.clear();
        id_to_entity_.clear();
    }

    usize WorldStorage::total_count() const {
        return wormholes_.size() + artifacts_.size() +
            celestial_bodies_.size() + spaceships_.size();
    }

    bool WorldStorage::remove_by_id(i32 id) {
        auto it = id_to_entity_.find(id);
        if (it == id_to_entity_.end()) { return false; }

        BaseModel* ptr = it->second;
        id_to_entity_.erase(it);

        auto remove_from = [ptr](auto& vec) {
            auto vec_it = std::find_if(
            vec.begin(), vec.end(), [ptr](const auto& uptr) { return uptr.get() == ptr; }
            );
            if (vec_it != vec.end()) {
                vec.erase(vec_it);
                return true;
            }
            return false;
        };

        return remove_from(wormholes_) ||
               remove_from(artifacts_) ||
               remove_from(celestial_bodies_) ||
               remove_from(spaceships_);
    }

    template std::optional<Ref<Wormhole>> WorldStorage::get_by_id<Wormhole>(i32 id);
    template std::optional<Ref<Artifact>> WorldStorage::get_by_id<Artifact>(i32 id);
    template std::optional<Ref<CelestialBody>> WorldStorage::get_by_id<CelestialBody>(i32 id);
    template std::optional<Ref<Spaceship>> WorldStorage::get_by_id<Spaceship>(i32 id);

    template std::optional<CRef<Wormhole>> WorldStorage::get_by_id<Wormhole>(i32 id) const;
    template std::optional<CRef<Artifact>> WorldStorage::get_by_id<Artifact>(i32 id) const;
    template std::optional<CRef<CelestialBody>> WorldStorage::get_by_id<CelestialBody>(i32 id) const;
    template std::optional<CRef<Spaceship>> WorldStorage::get_by_id<Spaceship>(i32 id) const;

}