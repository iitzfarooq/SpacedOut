#pragma once

#include "models/base.hpp"
#include "utils/types.hpp"

namespace ii {
    /**
     * Artifact: Stationary point object
     * AF(position) = Object permanently at position, pos(t) = position ∀t
     */
    struct Artifact : public BaseModel {
        const Vec2d position;

        Artifact(i32 id, std::string name, const Vec2d& position);
        virtual ~Artifact() = default;

        Vec2d pos(f64 t) const;
    };
}