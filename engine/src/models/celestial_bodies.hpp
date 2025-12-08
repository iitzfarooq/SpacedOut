#pragma once

#include "utils/types.hpp"
#include "models/base.hpp"
#include "world/orbits.hpp"

namespace ii {
    /**
     * CelestialBody: Massive astronomical object with extent
     * RI: radius > 0 ∧ mass > 0
     */
    struct CelestialBody : public BaseModel {
        const f64 radius;
        const f64 mass;

        // @pre radius > 0 ∧ mass > 0
        CelestialBody(i32 id, std::string name, f64 radius, f64 mass);
        virtual ~CelestialBody() = default;
            
        virtual Vec2d pos(f64 time) const = 0;
        virtual Vec2d vel(f64 time) const = 0;
    };

    /**
     * OrbitalBody: Body following elliptical orbit
     * AF(orbit) = Body with pos(t) = orbit.pos(t), vel(t) = orbit.vel(t)
     * RI: CelestialBody.RI ∧ orbit ≠ nullptr
     */
    struct OrbitalBody : public CelestialBody {
        const Uptr<SimpleOrbit> orbit;

        // @pre radius > 0 ∧ mass > 0 ∧ orbit ≠ nullptr
        OrbitalBody(
            i32 id, std::string name, f64 radius, f64 mass, 
            Uptr<SimpleOrbit> orbit
        );

        virtual Vec2d pos(f64 time) const override;
        virtual Vec2d vel(f64 time) const override;
    };

    /**
     * StationaryBody: Body at fixed position with zero velocity
     * AF(position) = Body at constant position, vel(t) = (0,0) ∀t
     * RI: CelestialBody.RI
     */
    struct StationaryBody : public CelestialBody {
        const Vec2d position;

        // @pre radius > 0 ∧ mass > 0
        StationaryBody(
            i32 id, std::string name, f64 radius, f64 mass, 
            Vec2d position
        );

        virtual Vec2d pos(f64 time) const override;
        virtual Vec2d vel(f64 time) const override;
    };
}