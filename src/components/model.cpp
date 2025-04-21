#include <vulkan/vulkan_raii.hpp>

#include "arcticvox/components/model.hpp"
#include "arcticvox/components/vertex.hpp"
#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/io/model_builder.hpp"

namespace arcticvox::components {

model::model(graphics::gpu_driver& driver, const io::model_builder& vertices) :
    driver_(driver),
    vertex_count_(vertices.vertices.size()),
    vertex_buffer_(nullptr),
    vertex_buffer_memory_(nullptr),
    indices_count_(vertices.indices.size()),
    indices_buffer_(nullptr),
    indices_buffer_memory_(nullptr),
    has_index_buffer(vertices.indices.size() > 0U) {
    create_vertex_buffers(vertices.vertices);
    create_index_buffers(vertices.indices);
}
void model::create_vertex_buffers(const std::vector<vertex>& vertices) {
    if(vertices.size() < 3U)
        throw std::runtime_error("Vertex count must be at least 3");

    const std::size_t buffer_sz = sizeof(vertices.front()) * vertices.size();
    vk::raii::Buffer staging_buffer =
        driver_.create_buffer(buffer_sz, vk::BufferUsageFlagBits::eTransferSrc);
    vk::raii::DeviceMemory staging_buffer_memory = driver_.bind_memory_to_buffer(
        staging_buffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    // copy data to shared memory
    void* data =
        staging_buffer_memory.mapMemory(0U, buffer_sz, static_cast<vk::MemoryMapFlags>(0U));
    std::memcpy(data, vertices.data(), static_cast<std::size_t>(buffer_sz));
    staging_buffer_memory.unmapMemory();

    vertex_buffer_ = driver_.create_buffer(
        buffer_sz, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst);
    vertex_buffer_memory_ =
        driver_.bind_memory_to_buffer(vertex_buffer_, vk::MemoryPropertyFlagBits::eDeviceLocal);
    driver_.copy_buffer(staging_buffer, vertex_buffer_, buffer_sz);
}

void model::create_index_buffers(const std::vector<uint32_t>& indices) {
    if(indices.empty())
        return;

    const std::size_t buffer_sz = sizeof(indices.front()) * indices.size();
    vk::raii::Buffer staging_buffer =
        driver_.create_buffer(buffer_sz, vk::BufferUsageFlagBits::eTransferSrc);
    vk::raii::DeviceMemory staging_buffer_memory = driver_.bind_memory_to_buffer(
        staging_buffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    // copy data to shared memory
    void* data =
        staging_buffer_memory.mapMemory(0U, buffer_sz, static_cast<vk::MemoryMapFlags>(0U));
    std::memcpy(data, indices.data(), static_cast<std::size_t>(buffer_sz));
    staging_buffer_memory.unmapMemory();

    indices_buffer_ = driver_.create_buffer(
        buffer_sz, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);
    indices_buffer_memory_ =
        driver_.bind_memory_to_buffer(indices_buffer_, vk::MemoryPropertyFlagBits::eDeviceLocal);
    driver_.copy_buffer(staging_buffer, indices_buffer_, buffer_sz);
}

void model::draw(vk::raii::CommandBuffer& command_buffer) {
    if(has_index_buffer)
        command_buffer.drawIndexed(indices_count_, 1U, 0U, 0U, 0U);
    else
        command_buffer.draw(vertex_count_, 1U, 0U, 0U);
}

void model::bind(vk::raii::CommandBuffer& command_buffer) {
    command_buffer.bindVertexBuffers(0U, {vertex_buffer_}, {0U});
    if(has_index_buffer)
        command_buffer.bindIndexBuffer(indices_buffer_, 0U, vk::IndexType::eUint32);
}

}
