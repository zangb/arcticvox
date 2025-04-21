#ifndef ARCTICVOX_MODEL_BUILDER_HPP
#define ARCTICVOX_MODEL_BUILDER_HPP

#include <cstdint>
#include <filesystem>
#include <vector>

#include "arcticvox/components/vertex.hpp"

namespace arcticvox::io {

struct model_builder {
    std::vector<components::vertex> vertices {};
    std::vector<uint32_t> indices {};

    [[nodiscard]] bool load_model(const std::filesystem::path& filepath);
};
}

#endif
