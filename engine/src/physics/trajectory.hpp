#pragma once

#include <numbers>
#include <cmath>
#include "utils/types.hpp"
#include "utils/math.hpp"

namespace ii {
    // Trajectory: Interface for motion path
    struct Trajectory {
        virtual ~Trajectory() = default;
        virtual Vec2d pos(f64 time) const = 0;
        virtual Vec2d vel(f64 time) const = 0;
        virtual f64 period() const = 0;
    };

    /**
     * EllipticalTrajectory: Parametric elliptical orbit
     * AF(a,b,ω,φ,θ,c) = Ellipse where pos(t) = R(θ)·[a·cos(ωt+φ), b·sin(ωt+φ)] + c
     * RI: a ≥ 0 ∧ b ≥ 0 ∧ w > 0
     * period() = 2π/ω
     */
    struct EllipticalTrajectory : public Trajectory {
        const f64 a, b, w, phi, rotation;
        const Vec2d center;

        EllipticalTrajectory(f64 a, f64 b, f64 w, f64 phi, f64 rotation, const Vec2d& center);

        Vec2d pos(f64 time) const override;
        Vec2d vel(f64 time) const override;
        f64 period() const override;
    };

    /**
     * StationaryTrajectory: Fixed position in space
     * AF(pos) = Constant position 'pos', zero velocity
     */
    struct StationaryTrajectory : public Trajectory {
        const Vec2d position;

        explicit StationaryTrajectory(const Vec2d& position);

        Vec2d pos(f64 time) const override;
        Vec2d vel(f64 time) const override;
        f64 period() const override;
    };
}
