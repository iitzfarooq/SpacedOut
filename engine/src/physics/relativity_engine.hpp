#pragma once

#include "utils/math.hpp"
#include "utils/types.hpp"
#include "physics/gravity_engine.hpp"

namespace ii {
    class RelativityEngine {
    public:
        explicit RelativityEngine(const GravityEngine& gm);
        ~RelativityEngine() = default;

        f64 gamma_at(const Vec2d& x, const f64 t) const;
        f64 inv_gamma_at(const Vec2d& x, const f64 t) const;

        f64 dtau_at(const f64 dt, const Vec2d& x, const f64 t) const;

    private:
        const GravityEngine& gm_;
    };
}