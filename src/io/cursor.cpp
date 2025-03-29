#include <cmath>

#include <GLFW/glfw3.h>

#include "arcticvox/graphics/window.hpp"
#include "arcticvox/io/cursor.hpp"

namespace arcticvox::io {

cursor::cursor(GLFWwindow* glfw_window) : glfw_(glfw_window) {
    glfwSetInputMode(glfw_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetCursorPosCallback(glfw_, cursor_position_cb);
}

cursor_mode cursor::get_current_cursor_mode() const {
    return static_cast<cursor_mode>(glfwGetInputMode(glfw_, GLFW_CURSOR));
}

cursor_delta cursor::get_cursor_delta() const {
    double delta_x = x_pos_ - prev_x_pos_;
    double delta_y = y_pos_ - prev_y_pos_;
    delta_x = std::abs(delta_x) > 1.0 ? delta_x : 0.0;
    delta_y = std::abs(delta_y) > 1.0 ? delta_y : 0.0;
    return {delta_x, delta_y};
}

void cursor::set_cursor_mode(const arcticvox::io::cursor_mode mode) {
    glfwSetInputMode(glfw_, GLFW_CURSOR, static_cast<int>(mode));
}

void cursor::update_position(double x, double y) {
    prev_x_pos_ = x_pos_;
    prev_y_pos_ = y_pos_;

    x_pos_ = x;
    y_pos_ = y;
}

void cursor::cursor_position_cb(GLFWwindow* window, double x_pos, double y_pos) {
    auto* w = reinterpret_cast<arcticvox::graphics::window*>(window);
    cursor& c = w->get_cursor();
    c.update_position(x_pos, y_pos);
}
}
