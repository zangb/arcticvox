#ifndef ARCTICVOX_PIPELINE_HPP
#define ARCTICVOX_PIPELINE_HPP

#include <unistd.h>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/gpu.hpp"

namespace arcticvox::graphics {

struct pipeline_config_info {
    vk::PipelineViewportStateCreateInfo viewport_info {};
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info {};
    vk::PipelineRasterizationStateCreateInfo rasterization_info {};
    vk::PipelineMultisampleStateCreateInfo multisample_info {};
    vk::PipelineColorBlendAttachmentState colourblend_attachment {};
    vk::PipelineColorBlendStateCreateInfo colourblend_info {};
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_info {};
    std::vector<vk::DynamicState> dynamic_states_enabled {};
    vk::PipelineDynamicStateCreateInfo dynamic_state_info {};

    vk::PipelineLayout pipeline_layout {};
    vk::RenderPass render_pass {};
    uint32_t subpass;
};

class pipeline {
  public:
    /**
     * @brief Constructs the pipeline object
     *
     * @param device The GPU being used
     * @param driver The driver interface for the GPU that is being used
     * @param vertex_shader The vertex shader to use
     * @param fragment_shader The fragment shader to use
     * @param config The pipeline configuration information
     */
    pipeline(gpu& device,
             gpu_driver& driver,
             const std::vector<char>& vertex_shader,
             const std::vector<char>& fragment_shader,
             const pipeline_config_info& config);

    pipeline(const pipeline& other) = delete;
    pipeline(pipeline&& other) = delete;

    ~pipeline() = default;

    pipeline& operator=(const pipeline& other) = delete;
    pipeline& operator=(pipeline&& other) = delete;

    static auto get_default_pipeline_config() -> pipeline_config_info;

    /**
     * @brief Returns the underlying vulkan pipeline
     */
    [[nodiscard]] auto vk_pipeline() -> vk::raii::Pipeline& {
        return pipeline_;
    };

  private:
    /**
     * @brief Creates a vulkan pipeline with the provided configuration
     *
     * @param config The configuration to use for the pipeline creation
     * @return A vulkan pipeline
     */
    [[nodiscard]] auto create_pipeline(const pipeline_config_info& config) -> vk::raii::Pipeline;

    /**
     * @brief Creates a vulkan shader module with the provided shader code
     *
     * @param shader_code The shader code to use for the module creation
     * @return A vulkan shader module
     */
    [[nodiscard]] auto create_shader_module(const std::vector<char>& shader_code)
        -> vk::raii::ShaderModule;

    gpu& device_;                                      //!< The gpu that's being used
    gpu_driver& driver_;                               //!< The driver to interface with the gpu
    const pipeline_config_info& config_;               //!< The pipeline's configuration
    vk::raii::ShaderModule vertex_shader_module_;      //!< The vertex shader module
    vk::raii::ShaderModule fragment_shader_module_;    //!< The fragment shader module

    vk::raii::Pipeline pipeline_;                      //!< The underlying vulkan pipeline object
};

}

#endif
