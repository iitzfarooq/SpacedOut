#pragma once

#include "models/base.hpp"
#include "utils/types.hpp"

namespace ii {
    struct Wormhole : public BaseModel {
        const Vec2d entry, exit;
        f64 t_open, t_exit;
        
        Wormhole(
            i32 id, std::string name,
            const Vec2d& entry, const Vec2d& exit,
            f64 t_open, f64 t_exit
        );

        bool is_open(f64 t) const;
    };
}