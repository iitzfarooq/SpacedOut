#include "wormhole.hpp"

namespace ii {
    Wormhole::Wormhole(
        i32 id, std::string name,
        const Vec2d& entry, const Vec2d& exit,
        f64 t_open, f64 t_exit
    ) : BaseModel(id, std::move(name)),
        entry(entry), exit(exit),
        t_open(t_open), t_exit(t_exit)
    {}

    bool Wormhole::is_open(f64 t) const {
        return t >= t_open && t <= t_exit;
    }
}