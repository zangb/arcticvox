#include <cstddef>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "arcticvox/components/vertex.hpp"

namespace arcticvox::components {

auto vertex::get_binding_description() -> std::vector<vk::VertexInputBindingDescription> {
    return std::vector<vk::VertexInputBindingDescription> {
        {.binding = 0U, .stride = sizeof(vertex), .inputRate = vk::VertexInputRate::eVertex}};
}

auto vertex::get_attribute_description() -> std::vector<vk::VertexInputAttributeDescription> {
    return std::vector<vk::VertexInputAttributeDescription> {
        {.location = 0U,
         .binding = 0U,
         .format = vk::Format::eR32G32B32Sfloat,
         .offset = offsetof(vertex, position)},
        {.location = 1U,
         .binding = 0U,
         .format = vk::Format::eR32G32B32Sfloat,
         .offset = offsetof(vertex, colour)},
    };
}

}
