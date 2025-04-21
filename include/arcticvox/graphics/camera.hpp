#ifndef ARCTICVOX_CAMERA_HPP
#define ARCTICVOX_CAMERA_HPP

#include <chrono>

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

#include "arcticvox/components/camera_controller.hpp"

namespace arcticvox::graphics {
/**
 * @class camera
 * @brief
 *
 */
class camera {
  public:
    camera() = default;

    ~camera() = default;

    /**
     * @brief Generates an orthographic projection matrix from the values
     *
     * @param left The left side of the box (towards negative x-axis)
     * @param right The right side of the box (towards positive x-axis)
     * @param bottom The bottom side of the box (towards negative y-axis)
     * @param top The top side of the box (towards positive y-axis)
     * @param near The near plane (towards negative z-axis)
     * @param far The far plane (towards positive z-axis)
     *
     * @details The function generates an orthographic projection matrix from the provided
     * values. The function assumes the Vulkan coordinate system, z-positive is into the screen
     * y-positive is down, x-positive is right.
     */
    void set_orthographics_projection(const float left,
                                      const float right,
                                      const float bottom,
                                      const float top,
                                      const float near,
                                      const float far);

    /**
     * @brief Generates the perspective projection matrix
     *
     * @param fov_y The field of view in the y direction, in radians
     * @param aspect The aspect ratio (width / height)
     * @param near The near plane (towards negative z-axis)
     * @param far The far plane (towards positive z-axis)
     *
     * @details The generates the perspective projection matrix from the provided values.
     */
    void set_perspective_projection(
        const float fov_y, const float aspect, const float near, const float far);

    /**
     * @brief Set view direction of camera
     const *
     * @param position The camera's position
     * @param direction The direction from the target to the camera
     * @param up The camera's up vector
     *
     * @details Set the view direction camera to direction, where direction points from the target
     * to the camera
     */
    void set_view_direction(const glm::vec3& position,
                            const glm::vec3& direction,
                            const glm::vec3& up = glm::vec3 {0.0f, -1.0f, 0.0f});

    /**
     * @brief Sets the camera to look at a target
     *
     * @param position The camera's position
     * @param target The position of the target to look at
     * @param up The camera's up-vector
     *
     * @details Sets the camera to look at a target from a given position
     */
    void look_at(const glm::vec3& position,
                 const glm::vec3& target,
                 const glm::vec3& up = glm::vec3 {0.0f, -1.0f, 0.0f});

    /**
     * @brief Returns the camera's projection matrix
     *
     * @return Returns the camera's projection matrix
     *
     * @details This matrix maps from camera space to screen space
     */
    glm::mat4 projection_matrix() const {
        return projection_matrix_;
    }

    void set_camera_controller(components::camera_controller& controller) {
        controller_ = &controller;
    }

    void update(const std::chrono::microseconds dt) {
        if(!controller_)
            return;
        controller_->update(*this, dt);
    }

    /**
     * @brief Getter function for the camera's view matrix
     *
     * @return Returns the camera's view matrix
     *
     * @details This matrix maps from world space to camera space
     */
    glm::mat4 view_matrix() {
        return view_matrix_;
    }

  private:
    components::camera_controller* controller_;

    glm::mat4 projection_matrix_ {1.f};
    glm::mat4 view_matrix_ {1.f};
};
}

#endif
