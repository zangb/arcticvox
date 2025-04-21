#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <spdlog/spdlog.h>

#include "arcticvox/graphics/gpu.hpp"
#include "arcticvox/graphics/swapchain.hpp"

namespace arcticvox::graphics {

swapchain::swapchain(gpu& gpu, gpu_driver& driver, vk::Extent2D extent, const bool try_mailbox) :
    gpu_(gpu),
    driver_(driver),
    window_extent_(extent),
    try_mailbox_(try_mailbox),
    swapchain_(create_swapchain()),
    swapchain_images_(swapchain_.getImages()),
    swapchain_image_views_(create_swapchain_image_views(swapchain_images_.size())),
    render_pass_(create_renderpass()),
    depth_image_format_(find_depth_format()),
    depth_images_(create_depth_images(swapchain_images_.size())),
    depth_image_memories_(create_device_memories(swapchain_images_.size())),
    depth_image_views_(create_depth_image_views(swapchain_images_.size())),
    image_available_semaphores_(create_semaphores(MAX_FRAMES_IN_FLIGHT)),
    render_finished_semaphores_(create_semaphores(MAX_FRAMES_IN_FLIGHT)),
    in_flight_fences_(create_fences(MAX_FRAMES_IN_FLIGHT)),
    swapchain_framebuffers_(create_framebuffers(swapchain_images_.size())) { }

swapchain::swapchain(gpu& gpu,
                     gpu_driver& driver,
                     vk::Extent2D extent,
                     std::unique_ptr<swapchain> old_swapchain,
                     const bool try_mailbox) :
    gpu_(gpu),
    driver_(driver),
    window_extent_(extent),
    try_mailbox_(try_mailbox),
    old_swapchain_(std::move(old_swapchain)),
    swapchain_(create_swapchain()),
    swapchain_images_(swapchain_.getImages()),
    swapchain_image_views_(create_swapchain_image_views(swapchain_images_.size())),
    render_pass_(create_renderpass()),
    depth_image_format_(find_depth_format()),
    depth_images_(create_depth_images(swapchain_images_.size())),
    depth_image_memories_(create_device_memories(swapchain_images_.size())),
    depth_image_views_(create_depth_image_views(swapchain_images_.size())),
    image_available_semaphores_(create_semaphores(MAX_FRAMES_IN_FLIGHT)),
    render_finished_semaphores_(create_semaphores(MAX_FRAMES_IN_FLIGHT)),
    in_flight_fences_(create_fences(MAX_FRAMES_IN_FLIGHT)),
    swapchain_framebuffers_(create_framebuffers(swapchain_images_.size())) { }

auto swapchain::choose_extent2d(const vk::SurfaceCapabilitiesKHR& capabilities) const
    -> vk::Extent2D {
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    vk::Extent2D actual_extent = window_extent_;
    actual_extent.width =
        std::max(capabilities.minImageExtent.width,
                 std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height =
        std::max(capabilities.minImageExtent.height,
                 std::min(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
}

auto swapchain::create_depth_images(std::size_t count) const -> std::vector<vk::raii::Image> {
    vk::Format depth_format = find_depth_format();

    std::vector<vk::raii::Image> depth_images;

    for(std::size_t i = 0U; i < count; ++i) {
        vk::ImageCreateInfo image_info {.flags = {},
                                        .imageType = vk::ImageType::e2D,
                                        .format = depth_format,
                                        .extent {.width = swapchain_extent_.width,
                                                 .height = swapchain_extent_.height,
                                                 .depth = 1U},
                                        .mipLevels = 1U,
                                        .arrayLayers = 1U,
                                        .samples = vk::SampleCountFlagBits::e1,
                                        .tiling = vk::ImageTiling::eOptimal,
                                        .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                        .sharingMode = vk::SharingMode::eExclusive,
                                        .initialLayout = vk::ImageLayout::eUndefined};
        depth_images.push_back(vk::raii::Image(driver_.get().device(), image_info));
    }
    return depth_images;
}

auto swapchain::create_depth_image_views(const std::size_t count) const
    -> std::vector<vk::raii::ImageView> {
    vk::Format depth_format = find_depth_format();

    std::vector<vk::raii::ImageView> views;
    for(std::size_t i = 0U; i < count; ++i) {
        vk::ImageViewCreateInfo image_view_info {
            .image = depth_images_.at(i),
            .viewType = vk::ImageViewType::e2D,
            .format = depth_format,
            .subresourceRange {.aspectMask = vk::ImageAspectFlagBits::eDepth,
                               .baseMipLevel = 0U,
                               .levelCount = 1U,
                               .baseArrayLayer = 0U,
                               .layerCount = 1U}};
        views.push_back(vk::raii::ImageView(driver_.get().device(), image_view_info));
    }
    return views;
}

auto swapchain::acquire_next_image() -> std::pair<vk::Result, uint32_t> {
    vk::Result wait_result = driver_.get().device().waitForFences(
        {in_flight_fences_[current_frame_]}, vk::True, std::numeric_limits<uint32_t>::max());

    if(wait_result != vk::Result::eSuccess)
        return {wait_result, 0U};
    return swapchain_.acquireNextImage(std::numeric_limits<uint32_t>::max(),
                                       image_available_semaphores_[current_frame_]);
}

auto swapchain::choose_present_mode(const std::vector<vk::PresentModeKHR>& modes,
                                    const bool try_mailbox) -> vk::PresentModeKHR {
    if(try_mailbox) {
        for(const auto& mode: modes) {
            if(mode == vk::PresentModeKHR::eMailbox) {
                spdlog::info("Present Mode: Mailbox");
                return mode;
            }
        }
    }

    return vk::PresentModeKHR::eFifo;
}

auto swapchain::choose_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats)
    -> vk::SurfaceFormatKHR {
    for(const auto& format: formats) {
        if((format.format == vk::Format::eB8G8R8A8Srgb)
           && (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear))
            return format;
    }
    return formats[0];
}

auto swapchain::create_device_memories(const std::size_t count) const
    -> std::vector<vk::raii::DeviceMemory> {
    std::vector<vk::raii::DeviceMemory> buffers;

    vk::PhysicalDeviceMemoryProperties memory_properties =
        gpu_.get().physical_device().getMemoryProperties();

    for(std::size_t i = 0U; i < count; ++i) {
        vk::MemoryRequirements memory_requirements = depth_images_.at(i).getMemoryRequirements();

        uint32_t type_bits = memory_requirements.memoryTypeBits;
        uint32_t type_index {~0U};

        for(uint32_t type_count = 0U; i < memory_properties.memoryTypeCount; ++type_count) {
            if((type_bits & 1U)
               && ((memory_properties.memoryTypes[type_count].propertyFlags
                    & vk::MemoryPropertyFlagBits::eDeviceLocal)
                   == vk::MemoryPropertyFlagBits::eDeviceLocal)) {
                type_index = type_count;
                break;
            }
            type_bits >>= 1;
        }

        if(type_index == uint32_t {~0U})
            throw std::runtime_error("Failed to setup depth buffer");

        vk::MemoryAllocateInfo memory_allocate_info {.allocationSize = memory_requirements.size,
                                                     .memoryTypeIndex = type_index};
        buffers.push_back(vk::raii::DeviceMemory(driver_.get().device(), memory_allocate_info));

        // bind memory to image
        depth_images_.at(i).bindMemory(buffers.back(), 0U);
    }
    return buffers;
}

auto swapchain::create_fences(const std::size_t count) const -> std::vector<vk::raii::Fence> {
    std::vector<vk::raii::Fence> fences;
    vk::FenceCreateInfo fence_info {.flags = vk::FenceCreateFlagBits::eSignaled};

    for(std::size_t i = 0U; i < count; ++i)
        fences.push_back(vk::raii::Fence(driver_.get().device(), fence_info));
    return fences;
}

auto swapchain::create_framebuffers(const std::size_t count) const
    -> std::vector<vk::raii::Framebuffer> {
    std::vector<vk::raii::Framebuffer> framebuffers;

    for(std::size_t i = 0U; i < count; ++i) {
        std::array<vk::ImageView, 2U> attachments = {swapchain_image_views_.at(i),
                                                     depth_image_views_.at(i)};
        vk::FramebufferCreateInfo framebuffer_info {.flags = {},
                                                    .renderPass = *render_pass_,
                                                    .attachmentCount =
                                                        static_cast<uint32_t>(attachments.size()),
                                                    .pAttachments = attachments.data(),
                                                    .width = swapchain_extent_.width,
                                                    .height = swapchain_extent_.height,
                                                    .layers = 1U};
        framebuffers.push_back(vk::raii::Framebuffer(driver_.get().device(), framebuffer_info));
    }
    return framebuffers;
}

auto swapchain::create_renderpass() -> vk::raii::RenderPass {
    vk::AttachmentDescription depth_attachment {
        .flags = {},
        .format = find_depth_format(),
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal};

    vk::AttachmentReference depth_attachment_ref {
        .attachment = 1U, .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal};

    vk::AttachmentDescription colour_attachment {.flags = {},
                                                 .format = swapchain_image_format_,
                                                 .samples = vk::SampleCountFlagBits::e1,
                                                 .loadOp = vk::AttachmentLoadOp::eClear,
                                                 .storeOp = vk::AttachmentStoreOp::eStore,
                                                 .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                                                 .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                                 .initialLayout = vk::ImageLayout::eUndefined,
                                                 .finalLayout = vk::ImageLayout::ePresentSrcKHR};

    vk::AttachmentReference colour_attachment_ref {
        .attachment = 0U, .layout = vk::ImageLayout::eColorAttachmentOptimal};

    vk::SubpassDescription subpass {.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
                                    .colorAttachmentCount = 1,
                                    .pColorAttachments = &colour_attachment_ref,
                                    .pDepthStencilAttachment = &depth_attachment_ref};
    vk::SubpassDependency dependency {
        .srcSubpass = vk::SubpassExternal,
        .dstSubpass = 0U,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                        | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                        | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask = static_cast<vk::AccessFlagBits>(0U),
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
                         | vk::AccessFlagBits::eDepthStencilAttachmentWrite};

    std::array<vk::AttachmentDescription, 2U> attachments {colour_attachment, depth_attachment};

    vk::RenderPassCreateInfo renderpass_info {.flags = {},
                                              .attachmentCount = attachments.size(),
                                              .pAttachments = attachments.data(),
                                              .subpassCount = 1U,
                                              .pSubpasses = &subpass,
                                              .dependencyCount = 1,
                                              .pDependencies = &dependency};

    return vk::raii::RenderPass {driver_.get().device(), renderpass_info};
}

auto swapchain::create_semaphores(const std::size_t count) const
    -> std::vector<vk::raii::Semaphore> {
    std::vector<vk::raii::Semaphore> semaphores;
    vk::SemaphoreCreateInfo semaphore_info {};

    for(std::size_t i = 0U; i < count; ++i)
        semaphores.push_back(vk::raii::Semaphore(driver_.get().device(), semaphore_info));
    return semaphores;
}

auto swapchain::create_swapchain() -> vk::raii::SwapchainKHR {
    swapchain_support_details swapchain_support = gpu_.get().query_swapchain_support();

    const vk::SurfaceFormatKHR surface_format =
        choose_surface_format(swapchain_support.surface_formats);

    const vk::PresentModeKHR present_mode =
        choose_present_mode(swapchain_support.present_modes, try_mailbox_);

    const vk::Extent2D extent = choose_extent2d(swapchain_support.surface_capabilities);

    swapchain_image_format_ = surface_format.format;
    swapchain_extent_ = extent;

    uint32_t image_count = swapchain_support.surface_capabilities.minImageCount + 1U;

    if((swapchain_support.surface_capabilities.maxImageCount > 0U)
       && (image_count > swapchain_support.surface_capabilities.maxImageCount))
        image_count = swapchain_support.surface_capabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR swapchain_info {
        .flags {},
        .surface {gpu_.get().surface()},
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1U,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices = nullptr,
        .preTransform = swapchain_support.surface_capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = present_mode,
        .clipped = vk::True,
        .oldSwapchain = old_swapchain_ ? *old_swapchain_->swapchain_ : nullptr};

    queue_family_indices indices = gpu_.get().find_queue_families();

    if(indices.graphics_family && indices.present_family
       && (indices.graphics_family != indices.present_family)) {
        std::array<uint32_t, 2U> family_indices = {*indices.graphics_family,
                                                   *indices.present_family};
        swapchain_info.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchain_info.queueFamilyIndexCount = 2U;
        swapchain_info.pQueueFamilyIndices = family_indices.data();
    }

    auto swapchain = vk::raii::SwapchainKHR {driver_.get().device(), swapchain_info};
    swapchain_image_format_ = surface_format.format;
    return swapchain;
}

auto swapchain::create_swapchain_image_views(const std::size_t count) const
    -> std::vector<vk::raii::ImageView> {
    std::vector<vk::raii::ImageView> views;

    for(std::size_t i = 0U; i < count; ++i) {
        vk::ImageViewCreateInfo image_view_info {
            .flags = {},
            .image = swapchain_images_.at(i),
            .viewType = vk::ImageViewType::e2D,
            .format = swapchain_image_format_,
            .subresourceRange {.aspectMask = vk::ImageAspectFlagBits::eColor,
                               .baseMipLevel = 0U,
                               .levelCount = 1U,
                               .baseArrayLayer = 0U,
                               .layerCount = 1U}};
        views.push_back(vk::raii::ImageView {driver_.get().device(), image_view_info});
    }
    return views;
}

auto swapchain::find_depth_format() const -> vk::Format {
    return gpu_.get().find_supported_format(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

auto swapchain::submit_command_buffers(vk::raii::CommandBuffer& command_buffer,
                                       uint32_t& image_index) -> vk::Result {
    vk::PipelineStageFlags wait_stages {vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::SubmitInfo submit_info {
        .waitSemaphoreCount = 1U,
        .pWaitSemaphores = &(*image_available_semaphores_.at(current_frame_)),
        .pWaitDstStageMask = &wait_stages,
        .commandBufferCount = 1U,
        .pCommandBuffers = &(*command_buffer),
        .signalSemaphoreCount = 1U,
        .pSignalSemaphores = &(*render_finished_semaphores_.at(current_frame_)),
    };

    driver_.get().graphics_queue().submit(submit_info, *in_flight_fences_.at(current_frame_));

    vk::PresentInfoKHR present_info {
        .waitSemaphoreCount = 1U,
        .pWaitSemaphores = &(*render_finished_semaphores_.at(current_frame_)),
        .swapchainCount = 1U,
        .pSwapchains = &(*swapchain_),
        .pImageIndices = &image_index,
    };

    vk::Result result = driver_.get().present_queue().presentKHR(present_info);

    return result;
}
}
