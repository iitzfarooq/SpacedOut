#pragma once

#include <cmath>
#include <limits>
#include <numeric>
#include <numbers>
#include <ranges>

#include "utils/types.hpp"

namespace ii {

struct MathConfig {
    static constexpr f64 epsilon = 1e-12f;
    static constexpr f64 pi = std::numbers::pi;
    static constexpr f64 inv_pi = std::numbers::inv_pi;
    static constexpr f64 inf = std::numeric_limits<f64>::infinity();
    static constexpr f64 G = 6.67430e-11; // gravitational constant
    static constexpr f64 c = 299792458;   // speed of light in vacuum
    static constexpr f64 au = 1.495978707e11; // astronomical unit in meters
    
    MathConfig() = delete;
    
    static inline bool is_equal(f64 a, f64 b, f64 tol = epsilon) {
        return std::abs(a - b) <= tol;
    }

    static inline bool is_zero(f64 a, f64 tol = epsilon) {
        return is_equal(a, 0.0, tol);
    }

    static inline f64 safe_div(f64 n, f64 d, f64 default_value = 0.0) {
        return is_zero(d) ? default_value : n / d;
    }

    static inline f64 eps_div(f64 n, f64 d, f64 tol = epsilon) {
        return is_zero(d, tol) ? (n >= 0.0 ? inf : -inf) : n / d;
    }

    static inline f64 clamp(f64 value, f64 min, f64 max) {
        return std::max(min, std::min(max, value));
    }

    static inline f64 clamp_angle(f64 angle) {
        angle = std::fmod(angle, 2.0 * pi);
        if (angle < 0.0) {
            angle += 2.0 * pi;
        }
        return angle;
    }

    static inline f64 deg_to_rad(f64 degrees) {
        return degrees * (pi / 180.0);
    }

    static inline f64 rad_to_deg(f64 radians) {
        return radians * 180.0 * inv_pi;
    }

    static inline f64 normalize_angle(f64 angle) {
        angle = std::fmod(angle + pi, 2.0 * pi);
        if (angle < 0.0) {
            angle += 2.0 * pi;
        }
        return angle - pi;
    }

    static inline f64 round(f64 value) {
        return std::round(value);
    }

};

}