#ifndef ARCTICVOX_SWAPCHAIN_HPP
#define ARCTICVOX_SWAPCHAIN_HPP

#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "spdlog/spdlog.h"

#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/gpu.hpp"

namespace arcticvox::graphics {

class swapchain final {
  public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2U;

    swapchain(gpu& gpu, gpu_driver& driver, vk::Extent2D window_extent, bool try_mailbox = false);

    swapchain(gpu& gpu,
              gpu_driver& driver,
              vk::Extent2D window_extent,
              std::unique_ptr<swapchain> old_swapchain,
              bool try_mailbox = false);

    swapchain(swapchain&& other) = delete;
    swapchain(const swapchain& other) = delete;

    ~swapchain() = default;

    swapchain& operator=(swapchain&& other) = delete;
    swapchain& operator=(const swapchain& other) = delete;

    [[nodiscard]] auto acquire_next_image() -> std::pair<vk::Result, uint32_t>;

    [[nodiscard]] float aspect_ratio() const {
        return static_cast<float>(swapchain_extent_.width)
               / static_cast<float>(swapchain_extent_.height);
    }

    [[nodiscard]] vk::Framebuffer get_framebuffer(uint32_t index) {
        if(index >= swapchain_framebuffers_.size()) {
            spdlog::error("Invalid index {} for accessing framebuffers (size: {})",
                          index,
                          swapchain_framebuffers_.size());
            throw std::runtime_error("Invalid index for accessing framebuffers");
        }
        return *swapchain_framebuffers_[index];
    }

    [[nodiscard]] vk::Extent2D get_extent() const {
        return swapchain_extent_;
    }

    [[nodiscard]] auto submit_command_buffers(vk::raii::CommandBuffer& buffer,
                                              uint32_t& image_index) -> vk::Result;

    [[nodiscard]] auto render_pass() -> vk::raii::RenderPass& {
        return render_pass_;
    }

    auto reset_fence() -> void {
        driver_.get().device().resetFences(*in_flight_fences_[current_frame_]);
    }

    auto update_current_frame() -> void {
        current_frame_ = (++current_frame_) & MAX_FRAMES_IN_FLIGHT;
    }

  private:
    /**
     * @brief Chooses the fitting presentation mode for the surface
     *
     * @param modes The available presentation modes of the chosen surface
     * @param try_mailbox Whether to attempt ot use mailbox presentation over fifo
     */
    [[nodiscard]] static auto choose_present_mode(const std::vector<vk::PresentModeKHR>& modes,
                                                  bool try_mailbox = false) -> vk::PresentModeKHR;

    /**
     * @brief Chooses from the available formats the first one that matches our criteria
     *
     * @param formats The available formats
     */
    [[nodiscard]] static auto choose_surface_format(
        const std::vector<vk::SurfaceFormatKHR>& formats) -> vk::SurfaceFormatKHR;

    /**
     * @brief Chooses the fitting extent based on the surface's capabilities
     *
     * @param capabilities The capabilities of the surface that is used
     */
    [[nodiscard]] auto choose_extent2d(const vk::SurfaceCapabilitiesKHR& capabilities) const
        -> vk::Extent2D;

    [[nodiscard]] auto create_depth_images(std::size_t count) const -> std::vector<vk::raii::Image>;

    [[nodiscard]] auto create_depth_image_views(std::size_t count) const
        -> std::vector<vk::raii::ImageView>;

    [[nodiscard]] auto create_device_memories(std::size_t count) const
        -> std::vector<vk::raii::DeviceMemory>;

    [[nodiscard]] auto create_fences(std::size_t count) const -> std::vector<vk::raii::Fence>;

    [[nodiscard]] auto create_framebuffers(std::size_t count) const
        -> std::vector<vk::raii::Framebuffer>;

    [[nodiscard]] auto create_renderpass() -> vk::raii::RenderPass;

    [[nodiscard]] auto create_semaphores(std::size_t count) const
        -> std::vector<vk::raii::Semaphore>;

    [[nodiscard]] auto create_swapchain() -> vk::raii::SwapchainKHR;

    [[nodiscard]] auto create_swapchain_image_views(std::size_t count) const
        -> std::vector<vk::raii::ImageView>;

    [[nodiscard]] auto find_depth_format() const -> vk::Format;

    std::reference_wrapper<gpu> gpu_;
    std::reference_wrapper<gpu_driver> driver_;

    vk::Extent2D window_extent_;

    bool try_mailbox_ = false;
    std::unique_ptr<swapchain> old_swapchain_;

    vk::Format swapchain_image_format_;
    vk::Extent2D swapchain_extent_;

    //! Images in the swapchain, initialized by the swapchain itself in create_swapchain()
    vk::raii::SwapchainKHR swapchain_;
    std::vector<vk::Image> swapchain_images_;
    std::vector<vk::raii::ImageView> swapchain_image_views_;

    vk::raii::RenderPass render_pass_;

    vk::Format depth_image_format_;
    std::vector<vk::raii::Image> depth_images_;
    std::vector<vk::raii::DeviceMemory> depth_image_memories_;
    std::vector<vk::raii::ImageView> depth_image_views_;

    std::vector<vk::raii::Semaphore> image_available_semaphores_;
    std::vector<vk::raii::Semaphore> render_finished_semaphores_;
    std::vector<vk::raii::Fence> in_flight_fences_;

    std::vector<vk::raii::Framebuffer> swapchain_framebuffers_;

    std::size_t current_frame_ = 0U;
};

}

#endif
