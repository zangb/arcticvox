#include <cstdint>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_structs.hpp>

#include <GLFW/glfw3.h>

#include "arcticvox/graphics/window.hpp"
#include "arcticvox/io/cursor.hpp"

namespace arcticvox::graphics {

window::window(const size_t width, const size_t height, std::string_view name) :
    name_(name),
    width_(width),
    height_(height),
    glfw_(init_glfw_window(width, height, name)),
    cursor_(glfw_) {
    glfwSetWindowUserPointer(glfw_, this);
}

window::~window() {
    glfwDestroyWindow(glfw_);
    glfwTerminate();
}

void window::create_window_surface(VkInstance instance, VkSurfaceKHR& surface) {
    if(glfwCreateWindowSurface(instance, glfw_, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create GLFW window surface!");
}

arcticvox::io::cursor& window::get_cursor() {
    return cursor_;
}

vk::Extent2D window::get_extent() const {
    return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};
}

GLFWwindow* window::get_glfw_window_instance() {
    return glfw_;
}

std::vector<const char*> window::get_required_glfw_extensions() const {
    uint32_t extensions_size = 0U;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_size);

    return std::vector<const char*>(glfw_extensions, glfw_extensions + extensions_size);
}

void window::reset_framebuffer_resized_flag() {
    was_resized_flag_ = false;
}

bool window::should_close() const {
    return glfwWindowShouldClose(glfw_);
}

bool window::was_resized() const {
    return was_resized_flag_;
}

void window::framebuffer_resize_cb(GLFWwindow* window, int width, int height) {
    auto* w = reinterpret_cast<::arcticvox::graphics::window*>(glfwGetWindowUserPointer(window));
    w->width_ = width;
    w->height_ = height;
    w->was_resized_flag_ = true;
}

GLFWwindow* window::init_glfw_window(
    const size_t width, const size_t height, std::string_view name) {
    glfwInit();

    // GLFW_NO_API prevents the setup of an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow* w = glfwCreateWindow(width_, height_, name_.c_str(), nullptr, nullptr);
    glfwSetFramebufferSizeCallback(w, framebuffer_resize_cb);
    return w;
}

}
