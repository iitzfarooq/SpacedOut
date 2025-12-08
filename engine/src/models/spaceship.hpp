#pragma once

#include <vector>
#include "models/base.hpp"
#include "utils/types.hpp"

namespace ii {
    /**
     * Spaceship: Controllable spacecraft with propulsion
     * AF = Craft with total_mass = dry_mass + fuel, discrete thrust_levels, exhaust_speed
     * RI: dry_mass > 0 ∧ max_fuel ≥ 0 ∧ exhaust_speed ≥ 0 ∧ |thrust_levels| > 0 ∧ ∀t ∈ thrust_levels: t ≥ 0
     */
    struct Spaceship : public BaseModel {
        f64 dry_mass;
        f64 max_fuel;
        f64 exhaust_speed;
        std::vector<f64> thrust_levels;

        // @pre dry_mass > 0 ∧ max_fuel ≥ 0 ∧ exhaust_speed ≥ 0 ∧ |thrust_levels| > 0 ∧ ∀t ∈ thrust_levels: t ≥ 0
        Spaceship(
            i32 id, std::string name, f64 dry_mass, f64 max_fuel,
            const std::vector<f64>& thrust_levels, f64 exhaust_speed
        );
    };
}