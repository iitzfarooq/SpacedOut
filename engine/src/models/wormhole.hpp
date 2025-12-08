#pragma once

#include "models/base.hpp"
#include "utils/types.hpp"

namespace ii {
    /**
     * Wormhole: Bidirectional portal connecting entry ↔ exit, traversable for t ∈ [t_open, t_close]
     * AF(entry, exit, t_open, t_close) = Portal at entry connecting to exit, active when t_open ≤ t ≤ t_close
     * RI: t_open ≥ 0 ∧ t_close ≥ t_open
     */
    struct Wormhole : public BaseModel {
        const Vec2d entry, exit;
        f64 t_open, t_close;
        
        // @pre t_open ≥ 0 ∧ t_close ≥ t_open
        Wormhole(
            i32 id, std::string name,
            const Vec2d& entry, const Vec2d& exit,
            f64 t_open, f64 t_close
        );

        // Returns true iff t ∈ [t_open, t_close]
        bool is_open(f64 t) const;
    };
}