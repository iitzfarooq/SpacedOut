#include "celestial_bodies.hpp"
#include "utils/helpers.hpp"

namespace ii {
    CelestialBody::CelestialBody(i32 id, const std::string& name, f64 radius, f64 mass, Uptr<Trajectory> trajectory)
        : BaseModel{id, name}, radius{radius}, mass{mass}, trajectory{std::move(trajectory)} {
        req(radius > 0.0, "Radius must be positive");
        req(mass > 0.0, "Mass must be positive");
        req(this->trajectory != nullptr, "Trajectory must not be null");
    }

    Vec2d CelestialBody::pos(f64 time) const {
        return trajectory->pos(time);
    }

    Vec2d CelestialBody::vel(f64 time) const {
        return trajectory->vel(time);
    }

}