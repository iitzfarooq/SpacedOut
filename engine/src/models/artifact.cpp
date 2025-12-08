#include "artifact.hpp"

namespace ii {
    Artifact::Artifact(
        i32 id, const std::string& name,
        const Vec2d& position
    ) : BaseModel(id, name), position(position) {}

    Vec2d Artifact::pos(f64 t) const {
        return position;
    }
}