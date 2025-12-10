#include "trajectory.hpp"

namespace ii {
    EllipticalTrajectory::EllipticalTrajectory(
        f64 a, f64 b, f64 w, f64 phi, f64 rotation, const Vec2d& center
    )   : a{a}, b{b}, w{w}, phi{phi}, rotation{rotation}, center{center} {}

    Vec2d EllipticalTrajectory::pos(f64 time) const {
        Vec2d pos {
            a * std::cos(w * time + phi), 
            b * std::sin(w * time + phi)
        };

        Affine2d transform = Affine2d::Identity();
        transform.rotate(this->rotation).translate(this->center);

        return transform * pos;
    }

    Vec2d EllipticalTrajectory::vel(f64 time) const {
        Vec2d vel {
            a * w * std::sin(w * time + phi) * -1, 
            b * w * std::cos(w * time + phi)
        };

        Affine2d transform = Affine2d::Identity();
        transform.rotate(this->rotation);

        return transform * vel;
    }

    f64 EllipticalTrajectory::period() const {
        return 2.0 * std::numbers::pi / w;
    }

    StationaryTrajectory::StationaryTrajectory(const Vec2d& position) : position(position) {}

    Vec2d StationaryTrajectory::pos(f64 time) const { return position; }
    Vec2d StationaryTrajectory::vel(f64 time) const { return Vec2d{0.0, 0.0}; }
    f64 StationaryTrajectory::period() const { return MathConfig::inf; }

}
