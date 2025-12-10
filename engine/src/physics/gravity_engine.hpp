#pragma once

#include "utils/types.hpp"
#include "utils/math.hpp"
#include "physics/world_storage.hpp"

namespace ii {
    class GravityEngine {
    public:
        /**
         * Information holder for gravity at a specific point.
         * Gravity(potential, acceleration):
         * - potential: Gravitational potential at the point (∑ -GM/r)
         * - acceleration: Gravitational acceleration vector at the point (∑ -GM/r^2 r̂)
         */
        struct Gravity {
            const f64 potential;
            const Vec2d acceleration;
        };

        explicit GravityEngine(const WorldStorage& ws);
        Gravity gravity_at(const Vec2d& x, const f64 t) const;

    private:
        const WorldStorage& ws_;
    };
}
