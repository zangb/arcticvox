#ifndef ARCTICVOX_DRIVER_HPP
#define ARCTICVOX_DRIVER_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_enums.hpp>

#include "arcticvox/graphics/gpu.hpp"

namespace arcticvox::graphics {

class gpu_driver {
  public:
    gpu_driver(gpu& gpu);

    gpu_driver(const gpu_driver& other) = delete;
    gpu_driver(gpu_driver&& other) = delete;

    ~gpu_driver() = default;

    gpu_driver& operator=(const gpu_driver& other) = delete;
    gpu_driver& operator=(gpu_driver&& other) = delete;

    [[nodiscard]] auto bind_memory_to_buffer(
        vk::raii::Buffer& buffer, vk::MemoryPropertyFlags properties) -> vk::raii::DeviceMemory;

    [[nodiscard]] auto begin_single_time_commands() -> vk::raii::CommandBuffer;

    auto copy_buffer(vk::raii::Buffer& src, vk::raii::Buffer& dst, vk::DeviceSize size) -> void;

    [[nodiscard]] auto create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage)
        -> vk::raii::Buffer;

    [[nodiscard]] auto device() -> vk::raii::Device& {
        return device_;
    }

    auto end_single_time_commands(vk::raii::CommandBuffer& command_buffer) -> void;

    [[nodiscard]] auto graphics_queue() -> vk::raii::Queue& {
        return graphics_queue_;
    }

    [[nodiscard]] auto present_queue() -> vk::raii::Queue& {
        return present_queue_;
    }

    [[nodiscard]] auto command_pool() -> vk::raii::CommandPool& {
        return command_pool_;
    }

  private:
    [[nodiscard]] auto create_command_pool() -> vk::raii::CommandPool;

    [[nodiscard]] auto create_device() -> vk::raii::Device;

    gpu& gpu_;
    vk::raii::Device device_;

    vk::raii::Queue graphics_queue_;
    vk::raii::Queue present_queue_;

    vk::raii::CommandPool command_pool_;
};

}

#endif
