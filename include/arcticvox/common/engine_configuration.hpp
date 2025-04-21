#ifndef ARCTICVOX_ENGINE_CONFIGURATION_HPP
#define ARCTICVOX_ENGINE_CONFIGURATION_HPP

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.hpp>

struct engine_configuration {
    static constexpr const char* engine_name = "arcticvox";
    static constexpr uint32_t engine_version = 0U;
    const char* app_name;
    uint32_t app_version;
    std::vector<const char*> validation_layers;
    std::vector<const char*> device_extensions;
};

#endif
