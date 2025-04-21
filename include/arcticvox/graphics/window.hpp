#ifndef ARCTICVOX_WINDOW_HPP
#define ARCTICVOX_WINDOW_HPP

#include <string_view>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_structs.hpp>

#include <GLFW/glfw3.h>

#include "arcticvox/io/input.hpp"

namespace arcticvox::graphics {

class window final {
  public:
    /**
     * @brief Sets up a window
     *
     * @param width     The window's initial width
     * @param height    The window's initial height
     * @param name      The window's title
     */
    window(const size_t width, const size_t height, std::string_view name);

    window(const window& other) = delete;
    window& operator=(const window& other) = delete;

    ~window();

    /**
     * @brief Creates a GLFW window surface and throws a std::runtime_error if the creation fails
     *
     * @param instace   The vulkan instance object to use
     * @param surface   The vulkan surface to use as a base
     */
    void create_window_surface(VkInstance instace, VkSurfaceKHR& surface);

    /**
     * @brief Returns  a reference to the internally stored cursor object
     */
    arcticvox::io::cursor& get_cursor();

    /**
     * @brief Returns the current window's extent
     *
     * @return The current window's extent
     */
    [[nodiscard]] vk::Extent2D get_extent() const;

    /**
     * @brief Returns the current GLFW window instance managed by this class
     *
     * @return The current GLFW instance
     */
    [[nodiscard]] GLFWwindow* get_glfw_window_instance();

    /**
     * @brief Returns a list of required extensions required by GLFW
     */
    [[nodiscard]] std::vector<const char*> get_required_glfw_extensions() const;

    /**
     * @brief Resets the flag that is set when the framebuffer was resized.
     */
    void reset_framebuffer_resized_flag();

    /**
     * @brief Evaluates whether the window should close based on GLFW
     *
     * @return True if the window should close
     */
    [[nodiscard]] bool should_close() const;

    /**
     * @brief Evaluates if the window has been resized, which causes the swapchain to be regenerated
     *
     * @return True if the window was resized
     */
    [[nodiscard]] bool was_resized() const;

  private:
    /**
     * @brief GLFW callback for glfw specific errors
     *
     * @param error_code    The GLFW error code
     * @param description   The description associated with the error code
     */
    static void glfw_error_cb(int error_code, const char* description);

    /**
     * @brief GLFW callback function called when the framebuffer / window is being resized
     *
     * @param window The window instance
     * @param width The new window width
     * @param height The new window height
     */
    static void framebuffer_resize_cb(GLFWwindow* window, int width, int height);

    /**
     * @brief Initializes the GLFW window with the necessary hints
     *
     * @return A pointer to the GLFW window instance
     */
    GLFWwindow* init_glfw_window(const size_t width, const size_t height, std::string_view name);

    const std::string name_;           //!< The window's name
    size_t width_;                     //!< The window's width
    size_t height_;                    //!< The window's height
    GLFWwindow* glfw_ = nullptr;       //!< The pointer to the GLFW window instance
    arcticvox::io::cursor cursor_;     //!< The cursor attached to the GLFW window instance
    bool was_resized_flag_ = false;    //!< Flag that indicates whether the framebuffer was resized
};
}

#endif
