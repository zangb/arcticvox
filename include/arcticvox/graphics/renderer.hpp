#ifndef ARCTICVOX_RENDERER_HPP
#define ARCTICVOX_RENDERER_HPP

#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/gpu.hpp"
#include "arcticvox/graphics/swapchain.hpp"
#include "arcticvox/graphics/window.hpp"
namespace arcticvox::graphics {

class renderer final {
  public:
    renderer(gpu& gpu, gpu_driver& driver, window& window, bool try_mailbox);

    renderer(const renderer& other) = delete;
    renderer(renderer&& other) = delete;

    ~renderer() = default;

    renderer& operator=(const renderer& other) = delete;
    renderer& operator=(renderer&& other) = delete;

    [[nodiscard]] vk::raii::CommandBuffer* begin_frame();

    void begin_swapchain_renderpass(vk::raii::CommandBuffer& command_buffer);

    [[nodiscard]] vk::raii::CommandBuffer& current_command_buffer() {
        if(!is_frame_started_)
            throw std::runtime_error("Cannot get command buffer when frame is not in progess");
        return command_buffers_.at(current_frame_index_);
    }

    void end_frame();

    void end_swapchain_renderpass(vk::raii::CommandBuffer& command_buffer);

    [[nodiscard]] swapchain& get_swapchain() {
        return *swapchain_;
    }

  private:
    [[nodiscard]] vk::raii::CommandBuffers create_command_buffers(std::size_t count);

    void recreate_swapchain();

    gpu& gpu_;
    gpu_driver& driver_;
    window& window_;
    bool try_mailbox_ = false;

    std::unique_ptr<swapchain> swapchain_;

    vk::raii::CommandBuffers command_buffers_;

    uint32_t current_image_index_ = 0U;
    uint32_t current_frame_index_ = 0U;

    bool is_frame_started_ = false;
};

}

#endif
