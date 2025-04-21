#ifndef ARCTICVOX_RENDER_SYSTEM_HPP
#define ARCTICVOX_RENDER_SYSTEM_HPP

#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "arcticvox/components/gameobject.hpp"
#include "arcticvox/graphics/camera.hpp"
#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/gpu.hpp"
#include "arcticvox/graphics/pipeline.hpp"
#include "arcticvox/io/shaderloader.hpp"

namespace arcticvox::graphics {

class render_system final {
  public:
    render_system(gpu& gpu, gpu_driver& driver, vk::raii::RenderPass& renderpass);

    render_system(const render_system& other) = delete;
    render_system(render_system&& other) = delete;

    render_system& operator=(const render_system& other);
    render_system& operator=(render_system&& other);

    ~render_system() = default;

    void render_gameobjects(vk::raii::CommandBuffer& command_buffer,
                            std::vector<components::gameobject>& gameobjects,
                            camera& cam);

  private:
    vk::raii::PipelineLayout create_pipeline_layout();
    std::unique_ptr<pipeline> create_pipeline(vk::raii::RenderPass& renderpass);

    const std::vector<char> fgt_shader_ =
        io::shader_loader::load_from_file("shaders/fragment_shader.frag.spv");
    const std::vector<char> vtx_shader_ =
        io::shader_loader::load_from_file("shaders/vertex_shader.vert.spv");

    gpu& gpu_;
    gpu_driver& driver_;

    vk::raii::PipelineLayout pipeline_layout_;
    std::unique_ptr<pipeline> pipeline_;
};
}

#endif
