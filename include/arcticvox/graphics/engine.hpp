#ifndef ARCTICVOX_ENGINE_HPP
#define ARCTICVOX_ENGINE_HPP

#include <string_view>
#include <vector>

#include "arcticvox/components/gameobject.hpp"
#include "arcticvox/graphics/camera.hpp"
#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/render_system.hpp"
#include "arcticvox/graphics/renderer.hpp"
#include "arcticvox/graphics/window.hpp"

namespace arcticvox::graphics {
class graphics_engine {
  public:
    graphics_engine(uint32_t width, uint32_t height, std::string_view name);

    void run();

    void set_objects_to_render(std::vector<components::gameobject>& objects) {
        render_objects_ = &objects;
    }

    void set_camera(camera& cam) {
        camera_ = &cam;
    }

    gpu_driver& get_gpu_driver() {
        return driver_;
    }

    window& get_window() {
        return window_;
    }

  private:
    window window_;
    gpu gpu_;
    gpu_driver driver_;
    renderer renderer_;
    render_system render_sys_;

    camera* camera_ = nullptr;

    std::vector<components::gameobject>* render_objects_ = nullptr;
};
}
#endif
