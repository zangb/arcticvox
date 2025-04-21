#ifndef ARCTICVOX_TRANSFORM_HPP
#define ARCTICVOX_TRANSFORM_HPP

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace arcticvox::components {
struct transform {
    //! rotation expressed as quaternion
    glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
    //! scale of the object
    glm::vec3 scale;
    //! position in space
    glm::vec3 translation;

    glm::mat4 mat4() {
        glm::mat4 translate_mat = glm::translate(glm::mat4 {1.0f}, translation);
        glm::mat4 scale_mat = glm::scale(glm::mat4 {1.0f}, scale);
        glm::mat4 rot_mat = glm::toMat4(rotation);
        return translate_mat * rot_mat * scale_mat;
    };
};
}

#endif
