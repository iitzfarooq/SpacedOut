#include "spaceship.hpp"
#include "utils/helpers.hpp"

namespace ii {
    Spaceship::Spaceship(
        i32 id, std::string name, f64 dry_mass, f64 max_fuel,
        const std::vector<f64>& thrust_levels, f64 exhaust_speed
    ) : BaseModel(id, std::move(name)), 
        dry_mass{dry_mass},
        max_fuel{max_fuel},
        exhaust_speed{exhaust_speed},
        thrust_levels{thrust_levels} {
        req(dry_mass > 0.0, "Dry mass must be positive.");
        req(max_fuel >= 0.0, "Max fuel cannot be negative.");
        req(exhaust_speed >= 0.0, "Exhaust speed cannot be negative.");
        req(!thrust_levels.empty(), "Thrust levels must be provided.");
        for (const auto& thrust : thrust_levels) {
            req(thrust >= 0.0, "Thrust levels cannot be negative.");
        }
    }
}