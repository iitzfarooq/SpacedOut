#include "spaceship.hpp"
#include "utils/helpers.hpp"

namespace ii {
    Spaceship::Spaceship(
        i32 id, const std::string& name, f64 dry_mass, f64 max_fuel,
        const std::vector<f64>& thrust_levels, f64 exhaust_speed
    ) : BaseModel(id, name),
        dry_mass(dry_mass),
        max_fuel(max_fuel),
        thrust_levels(thrust_levels),
        exhaust_speed(exhaust_speed) {
        req(this->dry_mass > 0.0, "Dry mass must be positive.");
        req(this->max_fuel >= 0.0, "Max fuel cannot be negative.");
        req(this->exhaust_speed >= 0.0, "Exhaust speed cannot be negative.");
        req(!this->thrust_levels.empty(), "Thrust levels must be provided.");
        for (const auto& thrust : this->thrust_levels) {
            req(thrust >= 0.0, "Thrust levels cannot be negative.");
        }
    }
}