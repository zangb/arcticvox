#ifndef ARCTICVOX_FPS_CAMERA_CONTROLLER_HPP
#define ARCTICVOX_FPS_CAMERA_CONTROLLER_HPP

#include <chrono>

#include "arcticvox/components/camera_controller.hpp"
#include "arcticvox/graphics/window.hpp"

namespace arcticvox::graphics {
class camera;
}

namespace arcticvox::components {

class fps_camera_controller final : public camera_controller {
  public:
    fps_camera_controller(graphics::window& window);

    void update(graphics::camera& cam, const std::chrono::microseconds dt) override;

  private:
    static constexpr float DEFAULT_SENSITIVITY = 0.1f;
    static constexpr float DEFAULT_MOVEMENT_SPEED = 1.0f;    //!< forward speed in m/s
    static constexpr float DEFAULT_TURN_SPEED =
        0.1f;    //!< Turn speed of camera in radians per
                 //!< second,
                 //!< https://pmc.ncbi.nlm.nih.gov/articles/PMC7210075/
                 //!< puts it at 12.8 rad/s peak

    const glm::vec3 UP {0.0f, -1.0f, 0.0f};
    const glm::vec3 FORWARD {0.0f, 0.0f, 1.0f};
    const glm::vec3 RIGHT {1.0f, 0.0f, 0.0f};

    float sensitivity_ = DEFAULT_SENSITIVITY;
    float movement_speed_ = DEFAULT_MOVEMENT_SPEED;
    float turn_speed_ = DEFAULT_TURN_SPEED;

    float pitch_ = glm::radians(0.0f);
    float yaw_ = glm::radians(0.0f);

    glm::vec3 position_ {-2.0f, 0.0f, 0.0f};
    glm::vec3 forward_ {0.0f, 0.0f, 1.0f};
};
}

#endif
