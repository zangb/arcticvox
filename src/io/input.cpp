#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>

#include "arcticvox/graphics/window.hpp"
#include "arcticvox/io/input.hpp"

namespace arcticvox::io {

cursor::cursor(GLFWwindow* glfw_window) : glfw_(glfw_window) {
    glfwSetInputMode(glfw_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetCursorPosCallback(glfw_, cursor_position_cb);
}

cursor_mode cursor::get_current_cursor_mode() const {
    return static_cast<cursor_mode>(glfwGetInputMode(glfw_, GLFW_CURSOR));
}

auto cursor::get_cursor_delta() const -> glm::vec2 {
    double delta_x = current_position_.x - previous_position_.x;
    double delta_y = current_position_.y - previous_position_.y;

    delta_x = std::abs(delta_x) > 1.0 ? delta_x : 0.0;
    delta_y = std::abs(delta_y) > 1.0 ? delta_y : 0.0;
    return glm::vec2 {delta_x, delta_y};
}

void cursor::set_cursor_mode(const arcticvox::io::cursor_mode mode) {
    glfwSetInputMode(glfw_, GLFW_CURSOR, static_cast<int>(mode));
}

void cursor::update_position(const glm::vec2 pos) {
    previous_position_ = current_position_;
    current_position_ = pos;
}

void cursor::cursor_position_cb(GLFWwindow* window, double x_pos, double y_pos) {
    auto* w = reinterpret_cast<arcticvox::graphics::window*>(glfwGetWindowUserPointer(window));
    cursor& c = w->get_cursor();
    c.update_position(glm::vec2 {x_pos, y_pos});
}
}
