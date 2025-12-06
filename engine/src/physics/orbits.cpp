#include "orbits.hpp"

namespace ii::engine {
    SimpleOrbit::SimpleOrbit(
        f64 a, f64 b, f64 w, f64 phi, f64 rotation, Vec2d center
    )   : a{a}, b{b}, w{w}, phi{phi}, rotation{rotation}, center{center} {}

    Vec2d SimpleOrbit::pos(f64 time) const {
        Vec2d pos {
            a * std::cos(w * time + phi), 
            b * std::sin(w * time + phi)
        };

        Affine2d transform = (
            Translation2d{center} * 
            Rotation2d{rotation}
        );

        return transform * pos;
    }

    Vec2d SimpleOrbit::vel(f64 time) const {
        Vec2d vel {
            a * w * std::sin(w * time + phi) * -1, 
            b * w * std::cos(w * time + phi)
        };

        Affine2d transform = (
            Affine2d::Identity() *
            Rotation2d{rotation}
        );

        return transform * vel;
    }

    f64 SimpleOrbit::period() const {
        return 2.0 * std::numbers::pi / w;
    }
}