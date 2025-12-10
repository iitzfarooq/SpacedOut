#pragma once

#include "utils/types.hpp"
#include "models/base.hpp"
#include "world/trajectory.hpp"

namespace ii {
    /**
     * CelestialBody: Massive astronomical object with extent
     * AF(radius, mass, trajectory) = Body with given radius and mass, following trajectory.
     * RI: radius > 0 ∧ mass > 0 ∧ trajectory ≠ nullptr
     */
    struct CelestialBody : public BaseModel {
        const f64 radius = 0.0;
        const f64 mass = 0.0;
        const Uptr<Trajectory> trajectory;

        // @pre radius > 0 ∧ mass > 0 ∧ trajectory ≠ nullptr
        CelestialBody(i32 id, const std::string& name, f64 radius, f64 mass, Uptr<Trajectory> trajectory);
        virtual ~CelestialBody() = default;
            
        virtual Vec2d pos(f64 time) const;
        virtual Vec2d vel(f64 time) const;
    };
}