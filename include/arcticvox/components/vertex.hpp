#ifndef ARCTICVOX_VERTEX_HPP
#define ARCTICVOX_VERTEX_HPP

#include <vector>

#include <vulkan/vulkan.hpp>

#include <glm/vec3.hpp>

#include "arcticvox/components/colour.hpp"
#include "arcticvox/components/position.hpp"
#include "arcticvox/components/uv.hpp"

namespace arcticvox::components {

class vertex {
  public:
    static std::vector<vk::VertexInputBindingDescription> get_binding_description();
    static std::vector<vk::VertexInputAttributeDescription> get_attribute_description();

    position position;
    colour colour;
    glm::vec3 normal;
    uv uv;
};

}

#endif
