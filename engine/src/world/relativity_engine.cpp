#include "relativity_engine.hpp"

namespace ii {
    RelativityEngine::RelativityEngine(const GravityEngine& gm) : gm_(gm) {}

    f64 RelativityEngine::gamma_at(const Vec2d& x, const f64 t) const {
        return 1.0 / inv_gamma_at(x, t);
    }

    f64 RelativityEngine::inv_gamma_at(const Vec2d& x, const f64 t) const {
        const auto phi = gm_.gravity_at(x, t).potential;
        constexpr auto c2 = MathConfig::c * MathConfig::c;

        return std::sqrt(1.0 + 2.0 * phi / c2);
    }

    f64 RelativityEngine::dtau_at(const f64 dt, const Vec2d& x, const f64 t) const {
        auto integrator = MathConfig::Integrator<f64>();
        auto f = [&](f64 tau, const f64 ti) {
            return inv_gamma_at(x, ti);
        };
        return integrator.integrate(0.0, t, dt, f);
    }


}