#pragma once

#include <concepts>
#include <optional>
#include <ranges>
#include "utils/types.hpp"
#include "world/world_storage.hpp"

namespace ii {
    template <typename T>
    concept HasPosition = requires (const T& obj, f64 t) {
        { obj.pos(t) } -> std::same_as<Vec2d>;
    };

    template<typename T>
    concept HasVelocity = requires(const T& obj, f64 t) {
        { obj.vel(t) } -> std::same_as<Vec2d>;
    };

    template<typename T>
    concept CelestialLike = std::derived_from<T, CelestialBody>;

    /**
     * WorldQuery: read-only query API over a `WorldStorage` instance.
     * AF = provides lazy views and filters over stored entity collections
     *      (e.g. within_radius, within_rect, open_wormholes, massive_bodies).
     * RI = referenced `WorldStorage` (storage_) must outlive this WorldQuery.
     */
    class WorldQuery {
    private:
        const WorldStorage& storage_;

    public:
        explicit WorldQuery(const WorldStorage& storage);
        ~WorldQuery() = default;

        inline auto all_wormholes() const {
            return storage_.wormholes()
                | std::views::transform([](const auto& ptr) {
                    return std::cref<Wormhole>(*ptr);
                });
        }

        inline auto all_artifacts() const {
            return storage_.artifacts()
                | std::views::transform([](const auto& ptr) {
                    return std::cref<Artifact>(*ptr);
                });
        }

        inline auto all_celestial_bodies() const {
            return storage_.celestial_bodies()
                | std::views::transform([](const auto& ptr) {
                    return std::cref<CelestialBody>(*ptr);
                });
        }

        inline auto all_spaceships() const {
            return storage_.spaceships()
                | std::views::transform([](const auto& ptr) {
                    return std::cref<Spaceship>(*ptr);
                });
        }

        template<HasPosition T>
        auto within_radius(const Vec2d& center, f64 radius, f64 time) const {
            return get_all_of_type<T>()
                | std::views::filter([center, radius, time](CRef<T> obj) {
                    const Vec2d& pos = obj.get().pos(time);
                    return (pos - center).norm() <= radius;
                });
        }

        template<HasPosition T>
        auto within_rect(const Vec2d& min, const Vec2d& max, f64 time) const {
            return get_all_of_type<T>()
                | std::views::filter([min, max, time](CRef<T> obj) {
                    const Vec2d pos = obj.get().pos(time);
                    return (pos.x() >= min.x() && pos.x() <= max.x())
                        && (pos.y() >= min.y() && pos.y() <= max.y());
                });
        }

        inline auto open_wormholes(f64 time) const {
            return all_wormholes()
                | std::views::filter([time](CRef<Wormhole> wh) {
                    return wh.get().is_open(time);
                });
        }

        inline auto massive_bodies(f64 min_mass) const {
            return all_celestial_bodies()
                | std::views::filter([min_mass](CRef<CelestialBody> cb) {
                    return cb.get().mass >= min_mass;
                });
        }

        inline auto orbital_bodies() const {
            return storage_.celestial_bodies()
                | std::views::filter([](const Uptr<CelestialBody>& cb) {
                    return cb && dynamic_cast<const OrbitalBody*>(cb.get()) != nullptr;
                })
                | std::views::transform([](const Uptr<CelestialBody>& cb) {
                    return std::cref<OrbitalBody>(*static_cast<const OrbitalBody*>(cb.get()));
                });
        }

        inline auto stationary_bodies() const {
            return storage_.celestial_bodies()
                | std::views::filter([](const Uptr<CelestialBody>& cb) {
                    return cb && dynamic_cast<const StationaryBody*>(cb.get()) != nullptr;
                })
                | std::views::transform([](const Uptr<CelestialBody>& cb) {
                    return std::cref<StationaryBody>(*static_cast<const StationaryBody*>(cb.get()));
                });
        }

    private:
        template <typename T>
        auto get_all_of_type() const {
            if constexpr (std::is_same_v<T, Wormhole>) {
                return all_wormholes();
            } else if constexpr (std::is_same_v<T, Artifact>) {
                return all_artifacts();
            } else if constexpr (std::is_same_v<T, CelestialBody>) {
                return all_celestial_bodies();
            } else if constexpr (std::is_same_v<T, Spaceship>) {
                return all_spaceships();
            } else {
                return std::views::empty<CRef<T>>;
            }
        }
    };

}