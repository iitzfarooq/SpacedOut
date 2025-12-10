#include "gravity_engine.hpp"

namespace ii {
    GravityEngine::GravityEngine(const WorldStorage& ws) : ws_(ws) {}

    GravityEngine::Gravity GravityEngine::gravity_at(const Vec2d& x, const f64 t) const {
        f64 phi = 0.0;
        Vec2d a{0.0, 0.0};

        for (const auto& body : ws_.celestial_bodies()) {
            auto ri     = body->pos(t);
            auto mi     = body->mass;
            auto Ri     = (ri - x).eval();
            auto d      = Ri.norm();
            auto inv_d3 = MathConfig::eps_div(1.0, d*d*d);

            a   += Ri * (MathConfig::G * mi * inv_d3);
            phi -= MathConfig::eps_div(MathConfig::G * mi, d);
        }

        return Gravity{phi, a};
    }

}