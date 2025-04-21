#include <memory>
#include <stdexcept>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <GLFW/glfw3.h>

#include "arcticvox/graphics/gpu.hpp"
#include "arcticvox/graphics/renderer.hpp"
#include "arcticvox/graphics/swapchain.hpp"
#include "arcticvox/graphics/window.hpp"

namespace arcticvox::graphics {
renderer::renderer(gpu& gpu, gpu_driver& driver, window& window, const bool try_mailbox) :
    gpu_(gpu),
    driver_(driver),
    window_(window),
    swapchain_(std::make_unique<swapchain>(gpu, driver, window.get_extent(), try_mailbox)),
    command_buffers_(create_command_buffers(swapchain::MAX_FRAMES_IN_FLIGHT)) { }

vk::raii::CommandBuffer* renderer::begin_frame() {
    if(is_frame_started_)
        throw std::runtime_error("Cannot call begin_frame() while already in progress");

    vk::Result result;
    try {
        std::tie(result, current_image_index_) = swapchain_->acquire_next_image();
    } catch(const std::exception& e) {
        recreate_swapchain();
        return nullptr;
    }

    swapchain_->reset_fence();
    is_frame_started_ = true;

    if((result != vk::Result::eSuccess) && (result != vk::Result::eSuboptimalKHR))
        throw std::runtime_error("Failed to acquire swapchain image");

    vk::raii::CommandBuffer& command_buffer = current_command_buffer();
    command_buffer.begin({});
    return &command_buffer;
}

void renderer::begin_swapchain_renderpass(vk::raii::CommandBuffer& command_buffer) {
    if(!is_frame_started_)
        throw std::runtime_error("Cannot end frame while frame is not in progress");
    if(&command_buffer != &current_command_buffer())
        throw std::runtime_error(
            "Cannot begin render pass on command buffer from a different frame");

    vk::ClearColorValue clear_colour {.float32 {{0.01f, 0.01f, 0.01f, 0.1f}}};
    vk::ClearDepthStencilValue clear_depthstencil {1.0f, 0U};

    std::array<vk::ClearValue, 2U> clear_values {};
    clear_values[0].color = clear_colour;
    clear_values[1].depthStencil = clear_depthstencil;

    vk::RenderPassBeginInfo render_pass_begin_info {
        .renderPass = *swapchain_->render_pass(),
        .framebuffer = swapchain_->get_framebuffer(current_image_index_),
        .renderArea = {.offset = {0, 0}, .extent = swapchain_->get_extent()},
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data()};

    vk::Viewport viewport {.x = 0.0f,
                           .y = 0.0f,
                           .width = static_cast<float>(swapchain_->get_extent().width),
                           .height = static_cast<float>(swapchain_->get_extent().height),
                           .minDepth = 0.0f,
                           .maxDepth = 1.0f};

    vk::Rect2D scissor {{0, 0}, swapchain_->get_extent()};

    command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    command_buffer.setViewport(0U, viewport);
    command_buffer.setScissor(0U, scissor);
}

vk::raii::CommandBuffers renderer::create_command_buffers(const std::size_t count) {
    vk::CommandBufferAllocateInfo alloc_info {.commandPool = driver_.command_pool(),
                                              .level = vk::CommandBufferLevel::ePrimary,
                                              .commandBufferCount = static_cast<uint32_t>(count)};
    return vk::raii::CommandBuffers(driver_.device(), alloc_info);
}

void renderer::end_frame() {
    if(!is_frame_started_)
        throw std::runtime_error("Cannot end frame while frame is not in progress");
    vk::raii::CommandBuffer& command_buffer = current_command_buffer();
    command_buffer.end();

    try {
        if(swapchain_->submit_command_buffers(command_buffer, current_image_index_)
           != vk::Result::eSuccess)
            throw std::runtime_error("Failed to submite command buffer");

    } catch(const std::exception& e) {
        window_.reset_framebuffer_resized_flag();
        recreate_swapchain();
    }

    swapchain_->update_current_frame();
    is_frame_started_ = false;
    current_frame_index_ = (++current_frame_index_) % swapchain::MAX_FRAMES_IN_FLIGHT;
}

void renderer::end_swapchain_renderpass(vk::raii::CommandBuffer& command_buffer) {
    if(!is_frame_started_)
        throw std::runtime_error(
            "Cannot call end_swapchain_renderpass while frame is not in progress");

    if(&command_buffer != &current_command_buffer())
        throw std::runtime_error("Cannot end renderpass on command buffer from a different frame");
    command_buffer.endRenderPass();
}

void renderer::recreate_swapchain() {
    vk::Extent2D extent = window_.get_extent();
    while((extent.width == 0U) || (extent.height == 0U)) {
        extent = window_.get_extent();
        glfwWaitEvents();
    }

    driver_.device().waitIdle();
    if(swapchain_)
        swapchain_ = std::make_unique<swapchain>(
            gpu_, driver_, window_.get_extent(), std::move(swapchain_), try_mailbox_);
    else
        swapchain_ = std::make_unique<swapchain>(gpu_, driver_, window_.get_extent(), try_mailbox_);
}

}
