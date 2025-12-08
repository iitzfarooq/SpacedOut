#include "wormhole.hpp"
#include "utils/helpers.hpp"

namespace ii {
    Wormhole::Wormhole(
        i32 id, std::string name,
        const Vec2d& entry, const Vec2d& exit,
        f64 t_open, f64 t_close
    ) : BaseModel(id, std::move(name)),
        entry(entry), exit(exit),
        t_open(t_open), t_close(t_close) {
        req(t_open >= 0.0, "Wormhole opening time must be non-negative.");
        req(t_close >= t_open, "Wormhole exit time must be after opening time.");
    }

    bool Wormhole::is_open(f64 t) const {
        return t >= t_open && t <= t_close;
    }
}