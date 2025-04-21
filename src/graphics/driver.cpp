#include <algorithm>
#include <cstdint>
#include <set>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_structs.hpp>

#include "arcticvox/common/engine_configuration.hpp"
#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/gpu.hpp"

namespace arcticvox::graphics {

gpu_driver::gpu_driver(gpu& gpu) :
    gpu_(gpu),
    device_(create_device()),
    graphics_queue_(device_, gpu_.find_queue_families().graphics_family.value(), 0U),
    present_queue_(device_, gpu_.find_queue_families().present_family.value(), 0U),
    command_pool_(create_command_pool()) { }

auto gpu_driver::begin_single_time_commands() -> vk::raii::CommandBuffer {
    vk::CommandBufferAllocateInfo allocate_info {.commandPool = command_pool_,
                                                 .level = vk::CommandBufferLevel::ePrimary,
                                                 .commandBufferCount = 1U};
    vk::raii::CommandBuffer command_buffer =
        std::move(vk::raii::CommandBuffers(device_, allocate_info).front());
    vk::CommandBufferBeginInfo begin_info {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    command_buffer.begin(begin_info);
    return command_buffer;
}

auto gpu_driver::bind_memory_to_buffer(vk::raii::Buffer& buffer, vk::MemoryPropertyFlags properties)
    -> vk::raii::DeviceMemory {
    vk::MemoryRequirements memory_requirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo memory_alloc_info {
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = gpu_.find_memory_type(memory_requirements.memoryTypeBits, properties)};

    vk::raii::DeviceMemory memory = device_.allocateMemory(memory_alloc_info);
    buffer.bindMemory(memory, 0U);
    return memory;
}

auto gpu_driver::copy_buffer(vk::raii::Buffer& src, vk::raii::Buffer& dst, const vk::DeviceSize sz)
    -> void {
    vk::raii::CommandBuffer command_buffer = begin_single_time_commands();
    vk::BufferCopy copy_region {.srcOffset = 0U, .dstOffset = 0U, .size = sz};
    command_buffer.copyBuffer(*src, *dst, copy_region);
    end_single_time_commands(command_buffer);
}

auto gpu_driver::create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage)
    -> vk::raii::Buffer {
    vk::BufferCreateInfo buffer_create_info {
        .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive};
    return device_.createBuffer(buffer_create_info);
}

auto gpu_driver::create_command_pool() -> vk::raii::CommandPool {
    queue_family_indices indices = gpu_.find_queue_families();
    vk::CommandPoolCreateInfo command_pool_create_info {
        .flags = vk::CommandPoolCreateFlagBits::eTransient
                 | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = *indices.graphics_family};
    return vk::raii::CommandPool {device_, command_pool_create_info};
}

auto gpu_driver::create_device() -> vk::raii::Device {
    queue_family_indices queue_indices = gpu_.find_queue_families();

    float queue_priority = 1.f;
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos {};
    std::set<uint32_t> unique_families = {*queue_indices.graphics_family,
                                          *queue_indices.present_family};

    std::for_each(unique_families.begin(), unique_families.end(), [&](const uint32_t family) {
        queue_create_infos.push_back(
            vk::DeviceQueueCreateInfo {.flags = {},
                                       .queueFamilyIndex = family,
                                       .queueCount = 1U,
                                       .pQueuePriorities = &queue_priority});
    });

    engine_configuration& config = gpu_.get_engine_configuration();

    vk::PhysicalDeviceFeatures features {};
    features.samplerAnisotropy = true;

    vk::DeviceCreateInfo device_create_info {
        .flags = {},
        .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledLayerCount = static_cast<uint32_t>(config.validation_layers.size()),
        .ppEnabledLayerNames = config.validation_layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(config.device_extensions.size()),
        .ppEnabledExtensionNames = config.device_extensions.data(),
        .pEnabledFeatures = &features};

    return vk::raii::Device(gpu_.physical_device(), device_create_info);
}

auto gpu_driver::end_single_time_commands(vk::raii::CommandBuffer& command_buffer) -> void {
    command_buffer.end();
    vk::SubmitInfo submit_info {
        .commandBufferCount = 1U,
        .pCommandBuffers = &(*command_buffer),
    };
    graphics_queue_.submit(submit_info);
    graphics_queue_.waitIdle();
}

}
