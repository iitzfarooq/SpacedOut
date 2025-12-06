#include "spaceship.hpp"

namespace ii {
    Spaceship::Spaceship(
        i32 id, std::string name, f64 dry_mass, f64 max_fuel,
        const std::vector<f64>& thrust_levels, f64 exhaust_speed
    ) : BaseModel(id, std::move(name)), 
        dry_mass{dry_mass},
        max_fuel{max_fuel},
        exhaust_speed{exhaust_speed},
        thrust_levels{thrust_levels}
    {}
}