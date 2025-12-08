#include "celestial_bodies.hpp"
#include "utils/helpers.hpp"

namespace ii {
    CelestialBody::CelestialBody(i32 id, std::string name, f64 radius, f64 mass)
        : BaseModel{id, std::move(name)}, radius{radius}, mass{mass} {
        req(radius > 0.0, "Radius must be positive");
        req(mass > 0.0, "Mass must be positive");
    }

    OrbitalBody::OrbitalBody(
        i32 id, std::string name, f64 radius, f64 mass, 
        Uptr<SimpleOrbit> orbit
    ) : CelestialBody{id, std::move(name), radius, mass}, orbit{std::move(orbit)} {
        req(this->orbit != nullptr, "Orbit must not be null");
    }

    Vec2d OrbitalBody::pos(f64 time) const {
        return orbit->pos(time);
    }

    Vec2d OrbitalBody::vel(f64 time) const {
        return orbit->vel(time);
    }

    StationaryBody::StationaryBody(
        i32 id, std::string name, f64 radius, f64 mass, 
        Vec2d position
    ) : CelestialBody{id, std::move(name), radius, mass}, position{std::move(position)} {}

    Vec2d StationaryBody::pos(f64 /*time*/) const {
        return position;
    }

    Vec2d StationaryBody::vel(f64 /*time*/) const {
        return Vec2d{0.0, 0.0};
    }
}