#ifndef ARCTICVOX_PUSH_CONSTANT_HPP
#define ARCTICVOX_PUSH_CONSTANT_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace arcticvox::components {
struct push_constant_data {
    glm::mat4 transform {1.0f};
    alignas(16) glm::vec3 colour;
};
}

#endif
