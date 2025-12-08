#pragma once

#include <cassert>
#include <stdexcept>
#include <compare>
#include <string>

inline void req(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Requirement failed: " + message);
    }
}