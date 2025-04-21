#include <algorithm>
#include <chrono>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "arcticvox/components/fps_camera_controller.hpp"
#include "arcticvox/graphics/camera.hpp"
#include "arcticvox/io/input.hpp"

namespace arcticvox::components {

fps_camera_controller::fps_camera_controller(graphics::window& window) :
    camera_controller(window) { }

void fps_camera_controller::update(graphics::camera& cam, const std::chrono::microseconds dt) {
    glm::vec3 delta_rot {0.0f};
    GLFWwindow* glfw_window = window_.get_glfw_window_instance();
    io::cursor& cursor = window_.get_cursor();

    if(glfwGetMouseButton(window_.get_glfw_window_instance(), 1U) == GLFW_PRESS) {
        glm::vec2 delta = cursor.get_cursor_delta();
        delta_rot = glm::vec3 {-delta.y * sensitivity_,    // pitch
                               delta.x * sensitivity_,     // yaw
                               0.0f};
    }

    if(glfwGetKey(window_.get_glfw_window_instance(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if(cursor.get_current_cursor_mode() == io::cursor_mode::disabled)
            cursor.set_cursor_mode(io::cursor_mode::normal);
        else
            cursor.set_cursor_mode(io::cursor_mode::disabled);
    }

    glm::vec3 delta_translation {0.0f};

    if(glfwGetKey(glfw_window, keymap_.forward) == GLFW_PRESS) {
        delta_translation.z += movement_speed_;
    }
    if(glfwGetKey(glfw_window, keymap_.backward) == GLFW_PRESS) {
        delta_translation.z -= movement_speed_;
    }
    if(glfwGetKey(glfw_window, keymap_.right) == GLFW_PRESS) {
        delta_translation.x += movement_speed_;
    }
    if(glfwGetKey(glfw_window, keymap_.left) == GLFW_PRESS) {
        delta_translation.x -= movement_speed_;
    }
    if(glfwGetKey(glfw_window, keymap_.yaw_r) == GLFW_PRESS) {
        delta_rot.y += turn_speed_;
    }
    if(glfwGetKey(glfw_window, keymap_.yaw_l) == GLFW_PRESS) {
        delta_rot.y -= turn_speed_;
    }
    if(glfwGetKey(glfw_window, keymap_.pitch_u) == GLFW_PRESS) {
        delta_rot.x += turn_speed_;
    }
    if(glfwGetKey(glfw_window, keymap_.pitch_d) == GLFW_PRESS) {
        delta_rot.x -= turn_speed_;
    }
    delta_translation *= dt.count() * 10e-6;
    delta_rot *= dt.count() * 10e-6;

    pitch_ = std::clamp(pitch_ + delta_rot.x, -glm::half_pi<float>(), glm::half_pi<float>());
    yaw_ = glm::mod(yaw_ + delta_rot.y, glm::two_pi<float>());

    glm::quat qx = glm::angleAxis(pitch_, RIGHT);
    glm::quat qy = glm::angleAxis(yaw_, UP);

    position_ += qx * delta_translation * qy;
    forward_ = qx * FORWARD * qy;

    cam.set_view_direction(position_, forward_, UP);
}

}
