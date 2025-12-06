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

    class WorldQuery {
    private:
        const WorldStorage& storage_;

    public:
        explicit WorldQuery(const WorldStorage& storage);

        // ----------------------------------------------------
        // 1. TYPE-SPECIFIC VIEWS (returns ranges of raw pointers)
        // ----------------------------------------------------

        inline auto all_wormholes() const {
            return storage_.wormholes()
                | std::views::transform([](const auto& ptr) { return ptr.get(); });
        }

        inline auto all_artifacts() const {
            return storage_.artifacts()
                | std::views::transform([](const auto& ptr) { return ptr.get(); });
        }

        inline auto all_celestial_bodies() const {
            return storage_.celestial_bodies()
                | std::views::transform([](const auto& ptr) { return ptr.get(); });
        }

        inline auto all_spaceships() const {
            return storage_.spaceships()
                | std::views::transform([](const auto& ptr) { return ptr.get(); });
        }

        // ----------------------------------------------------
        // 2. SPATIAL QUERIES (for objects with position)
        // ----------------------------------------------------

        template<HasPosition T>
        auto within_radius(const Vec2d& center, f64 radius, f64 time) const {
            auto all_objects = get_all_of_type<T>();

            return all_objects
                | std::views::filter([center, radius, time] (const T* obj) {
                    if (!obj) return false;
                    const Vec2d pos = obj->pos(time);
                    return (pos - center).norm() <= radius;
                });
        }

        template<HasPosition T>
        auto within_rect(const Vec2d& min, const Vec2d& max, f64 time) const {
            auto all_objects = get_all_of_type<T>();

            return all_objects
                | std::views::filter([min, max, time] (const T* obj) {
                    if (!obj) return false;
                    const Vec2d pos = obj->pos(time);
                    return (pos.x() >= min.x() && pos.x() <= max.x())
                        && (pos.y() >= min.y() && pos.y() <= max.y());
                });
        }

        // ----------------------------------------------------
        // 3. FILTERED QUERIES
        // ----------------------------------------------------

        inline auto open_wormholes(f64 time) const {
            return storage_.wormholes()
                | std::views::filter([time](const Uptr<Wormhole>& wh) {
                    return wh && wh->is_open(time);
                })
                | std::views::transform([](const Uptr<Wormhole>& wh) {
                    return wh.get();
                });
        }

        inline auto massive_bodies(f64 min_mass) const {
            return storage_.celestial_bodies()
                | std::views::filter([min_mass](const Uptr<CelestialBody>& cb) {
                    return cb && cb->mass >= min_mass;
                })
                | std::views::transform([](const Uptr<CelestialBody>& cb) {
                    return cb.get();
                });
        }

        inline auto orbital_bodies() const {
            return storage_.celestial_bodies()
                | std::views::filter([](const Uptr<CelestialBody>& cb) {
                    return cb && dynamic_cast<const OrbitalBody*>(cb.get()) != nullptr;
                })
                | std::views::transform([](const Uptr<CelestialBody>& cb) {
                    return static_cast<const OrbitalBody*>(cb.get());
                });
        }

        inline auto stationary_bodies() const {
            return storage_.celestial_bodies()
                | std::views::filter([](const Uptr<CelestialBody>& cb) {
                    return cb && dynamic_cast<const StationaryBody*>(cb.get()) != nullptr;
                })
                | std::views::transform([](const Uptr<CelestialBody>& cb) {
                    return static_cast<const StationaryBody*>(cb.get());
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
                static_assert(false, "Unsupported type for WorldQuery::get_all_of_type");
            }
        }
    };

}