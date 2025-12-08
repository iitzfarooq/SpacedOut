#pragma once

#include <numbers>
#include <cmath>
#include "utils/types.hpp"

namespace ii {
    // Orbit: Interface for orbital motion
    struct Orbit {
        virtual ~Orbit() = default;
        virtual Vec2d pos(f64 time) const = 0;
        virtual Vec2d vel(f64 time) const = 0;
        virtual f64 period() const = 0;
    };

    /**
     * SimpleOrbit: Parametric elliptical orbit
     * AF(a,b,ω,φ,θ,c) = Ellipse where pos(t) = R(θ)·[a·cos(ωt+φ), b·sin(ωt+φ)] + c
     * RI: a ≥ 0 ∧ b ≥ 0 ∧ w > 0
     * period() = 2π/ω
     */
    struct SimpleOrbit : public Orbit {
        const f64 a, b, w, phi, rotation;
        const Vec2d center;

        SimpleOrbit(f64 a, f64 b, f64 w, f64 phi, f64 rotation, const Vec2d& center);

        Vec2d pos(f64 time) const override;
        Vec2d vel(f64 time) const override;
        f64 period() const override;
    };
}