#include "arcticvox/graphics/camera.hpp"

namespace arcticvox::graphics {

void camera::set_orthographics_projection(const float left,
                                          const float right,
                                          const float bottom,
                                          const float top,
                                          const float near,
                                          const float far) {
    projection_matrix_ = glm::mat4 {2.0f / (right - left),
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    2.0f / (top - bottom),
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    1.0f / (far - near),
                                    0.0f,
                                    -(right + left) / (right - left),
                                    -(top + bottom) / (top - bottom),
                                    -near / (far - near),
                                    1.0f};
}

void camera::set_perspective_projection(
    const float fov_y, const float aspect, const float near, const float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon() > 0.0f));
    const float tan_half_fovy = std::tan(fov_y / 2.0f);
    projection_matrix_ = glm::mat4 {1.0f / (aspect * tan_half_fovy),
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    1.0f / tan_half_fovy,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    far / (far - near),
                                    1.0f,
                                    0.0f,
                                    0.0f,
                                    -far * near / (far - near),
                                    0.0f};
}

void camera::set_view_direction(
    const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up) {
    assert(glm::dot(direction, direction) > std::numeric_limits<float>::epsilon());
    // create orthonormal basis
    glm::vec3 w {glm::normalize(direction)};            // vector pointing to camera
    glm::vec3 u {glm::normalize(glm::cross(w, up))};    // orthogonal vector to the left
    glm::vec3 v {glm::cross(w, u)};                     // the up vector normalized
    view_matrix_ = glm::mat4 {u.x,
                              v.x,
                              w.x,
                              0.0f,
                              u.y,
                              v.y,
                              w.y,
                              0.0f,
                              u.z,
                              v.z,
                              w.z,
                              0.0f,
                              -glm::dot(u, position),
                              -glm::dot(v, position),
                              -glm::dot(w, position),
                              1.0f};
}

void camera::look_at(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    set_view_direction(position, target - position, up);
}
}
