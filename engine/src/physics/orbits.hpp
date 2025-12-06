#pragma once

#include <numbers>
#include <cmath>
#include "utils/types.hpp"

namespace ii::engine {
    struct Orbit {
        virtual ~Orbit() = default;
        virtual Vec2d pos(f64 time) const = 0;
        virtual Vec2d vel(f64 time) const = 0;
        virtual f64 period() const = 0;
    };

    struct SimpleOrbit : public Orbit {
        const f64 a, b, w, phi, rotation;
        const Vec2d center;

        SimpleOrbit(f64 a, f64 b, f64 w, f64 phi, f64 rotation, Vec2d center);
        virtual Vec2d pos(f64 time) const override;
        virtual Vec2d vel(f64 time) const override;
        virtual f64 period() const override;
    };
}