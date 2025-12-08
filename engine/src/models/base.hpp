#pragma once

#include <string>
#include "utils/types.hpp"
#include "utils/helpers.hpp"

namespace ii {
    struct BaseModel {
        const i32 id;
        const std::string name;

        inline BaseModel(i32 id, std::string name) : id{id}, name{std::move(name)} {
            req(id >= 0, "ID must be non-negative.");
        }
        virtual ~BaseModel() = default;
    };
}