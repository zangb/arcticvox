#ifndef ARCTICVOX_MODEL_HPP
#define ARCTICVOX_MODEL_HPP

#include <cstdint>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "arcticvox/components/vertex.hpp"
#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/io/model_builder.hpp"

namespace arcticvox::components {

class model final {
  public:
    model(graphics::gpu_driver& driver, const io::model_builder& builder);

    model(const model& other) = delete;

    model& operator=(const model& other) = delete;

    ~model() = default;

    void draw(vk::raii::CommandBuffer& command_buffer);
    void bind(vk::raii::CommandBuffer& command_buffer);

  private:
    void create_vertex_buffers(const std::vector<vertex>& vertices);
    void create_index_buffers(const std::vector<uint32_t>& indices);

    void copy_vertex_data_to_memory_map(const std::vector<vertex>& vertices);
    void copy_index_data_to_memory_map(const std::vector<uint32_t>& indices);

    graphics::gpu_driver& driver_;
    std::size_t vertex_count_;
    vk::raii::Buffer vertex_buffer_;
    vk::raii::DeviceMemory vertex_buffer_memory_;

    std::size_t indices_count_;
    vk::raii::Buffer indices_buffer_;
    vk::raii::DeviceMemory indices_buffer_memory_;

    bool has_index_buffer {};
};

}

#endif
