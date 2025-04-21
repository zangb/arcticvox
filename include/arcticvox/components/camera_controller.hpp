#ifndef ARCTICVOX_CAMERA_CONTROLLER_HPP
#define ARCTICVOX_CAMERA_CONTROLLER_HPP

#include <chrono>
#include <cstdint>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "arcticvox/graphics/window.hpp"

namespace arcticvox::graphics {
class camera;
}

namespace arcticvox::components {

class camera_controller {
  public:
    struct keymap {
        uint32_t forward;
        uint32_t backward;
        uint32_t left;
        uint32_t right;
        uint32_t pitch_u;
        uint32_t pitch_d;
        uint32_t yaw_l;
        uint32_t yaw_r;
        uint32_t roll_l;
        uint32_t roll_r;
    };

    /**
     * @brief Constructor of the camera controller
     *
     * @param window The shk window from which to get user input
     * @param camera The camera which will be controlled
     */
    camera_controller(graphics::window& window) : keymap_(get_default_keymap()), window_(window) { }
    ~camera_controller() = default;

    void set_keymap(const keymap& map) {
        keymap_ = map;
    }

    /**
     * @brief Returns the default keymap for camera movement
     *
     * @return Returns the default keymap for camera movement
     */
    keymap get_default_keymap() const {
        return keymap {.forward = GLFW_KEY_W,
                       .backward = GLFW_KEY_S,
                       .left = GLFW_KEY_A,
                       .right = GLFW_KEY_D,
                       .pitch_u = GLFW_KEY_DOWN,
                       .pitch_d = GLFW_KEY_UP,
                       .yaw_l = GLFW_KEY_Q,
                       .yaw_r = GLFW_KEY_E,
                       .roll_l = GLFW_KEY_LEFT,
                       .roll_r = GLFW_KEY_RIGHT};
    }

    /**
     * @brief Updates the camera position based on user input
     */
    virtual void update(arcticvox::graphics::camera& cam, const std::chrono::microseconds dt) = 0;

  protected:
    keymap keymap_;
    graphics::window& window_;
};
}
#endif
