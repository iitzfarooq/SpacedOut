#pragma once

#include "utils/types.hpp"
#include "models/base.hpp"
#include "world/orbits.hpp"

namespace ii {
    struct CelestialBody : public BaseModel {
        const f64 radius;
        const f64 mass;

        CelestialBody(i32 id, std::string name, f64 radius, f64 mass);

        virtual ~CelestialBody() = default;
            
        virtual Vec2d pos(f64 time) const = 0;
        virtual Vec2d vel(f64 time) const = 0;
    };

    struct OrbitalBody : public CelestialBody {
        const Uptr<SimpleOrbit> orbit;

        OrbitalBody(
            i32 id, std::string name, f64 radius, f64 mass, 
            Uptr<SimpleOrbit> orbit
        );
        virtual Vec2d pos(f64 time) const override;
        virtual Vec2d vel(f64 time) const override;
    };

    struct StationaryBody : public CelestialBody {
        const Vec2d position;

        StationaryBody(
            i32 id, std::string name, f64 radius, f64 mass, 
            Vec2d position
        );
        virtual Vec2d pos(f64 time) const override;
        virtual Vec2d vel(f64 time) const override;
    };
}