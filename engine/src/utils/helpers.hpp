#pragma once

#include <cassert>
#include <stdexcept>
#include <compare>
#include <string>

#include "utils/helpers.hpp"

inline void req(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Requirement failed: " + message);
    }
}

template<typename T>
concept Enum = std::is_enum_v<T>;

template<Enum T>
struct EnumClassHasher {
    inline size_t operator() (const T& e) const noexcept {
        using U = std::underlying_type_t<T>;
        return std::hash<U>()(static_cast<U>(e));
    }
};