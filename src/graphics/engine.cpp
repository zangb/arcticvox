#include <chrono>
#include <string_view>

#include <vulkan/vulkan_raii.hpp>

#include "arcticvox/graphics/camera.hpp"
#include "arcticvox/graphics/engine.hpp"
#include "arcticvox/graphics/render_system.hpp"

namespace arcticvox::graphics {

graphics_engine::graphics_engine(
    const uint32_t width, const uint32_t height, std::string_view name) :
    window_(width, height, name),
    gpu_(name, 0, window_),
    driver_(gpu_),
    renderer_(gpu_, driver_, window_, false),
    render_sys_(gpu_, driver_, renderer_.get_swapchain().render_pass()) { }

void graphics_engine::run() {
    std::chrono::time_point<std::chrono::high_resolution_clock> current_time =
        std::chrono::high_resolution_clock::now();

    while(true) {
        const auto new_time = std::chrono::high_resolution_clock::now();
        const std::chrono::microseconds frame_time =
            std::chrono::duration_cast<std::chrono::microseconds>(new_time - current_time);
        current_time = new_time;

        // 50 degree fov

        if(camera_ && render_objects_) {
            camera_->update(frame_time);

            if(vk::raii::CommandBuffer* cmd_buffer = renderer_.begin_frame()) {
                renderer_.begin_swapchain_renderpass(*cmd_buffer);
                render_sys_.render_gameobjects(*cmd_buffer, *render_objects_, *camera_);
                renderer_.end_swapchain_renderpass(*cmd_buffer);
                renderer_.end_frame();
            }
        }

        if(camera_) {
            const float aspect = renderer_.get_swapchain().aspect_ratio();
            camera_->set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 10.f);
        }

        if(window_.should_close()) {
            spdlog::info("Window close triggered. Terminating application.");
            break;
        }
        glfwPollEvents();
    }
    driver_.device().waitIdle();
}

}
