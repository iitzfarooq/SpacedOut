#pragma once

#include <vector>
#include "models/base.hpp"
#include "utils/types.hpp"

namespace ii {
    struct Spaceship : public BaseModel {
        f64 dry_mass;
        f64 max_fuel;
        f64 exhaust_speed;
        std::vector<f64> thrust_levels;

        Spaceship(
            i32 id, std::string name, f64 dry_mass, f64 max_fuel,
            const std::vector<f64>& thrust_levels, f64 exhaust_speed
        );
    };
}