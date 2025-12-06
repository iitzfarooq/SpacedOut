#include "artifact.hpp"

namespace ii {
    Artifact::Artifact(
        i32 id, std::string name,
        const Vec2d& position
    ) : BaseModel(id, std::move(name)), position(position) {}

    Vec2d Artifact::pos(f64 t) const {
        return position;
    }
}