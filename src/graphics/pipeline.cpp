#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "arcticvox/components/vertex.hpp"
#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/pipeline.hpp"
#include "arcticvox/io/shaderloader.hpp"

namespace arcticvox::graphics {

pipeline::pipeline(gpu& device,
                   gpu_driver& driver,
                   const std::vector<char>& vertex_shader,
                   const std::vector<char>& fragment_shader,
                   const pipeline_config_info& config) :
    device_(device),
    driver_(driver),
    config_(config),
    vertex_shader_module_(create_shader_module(vertex_shader)),
    fragment_shader_module_(create_shader_module(fragment_shader)),
    pipeline_(create_pipeline(config)) { }

auto pipeline::get_default_pipeline_config() -> pipeline_config_info {
    static constexpr std::array<vk::DynamicState, 2U> dynamic_states_enabled {
        vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    static constexpr vk::PipelineColorBlendAttachmentState colourblend_attachment_state {
        .blendEnable = vk::False,
        .srcColorBlendFactor = vk::BlendFactor::eOne,
        .dstColorBlendFactor = vk::BlendFactor::eZero,
        .colorBlendOp = vk::BlendOp::eAdd,
        // mixing current output with what's already in the framebuffer
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                          | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,

    };

    pipeline_config_info config_info {
        .viewport_info {
            .viewportCount = 1U, .pViewports = nullptr, .scissorCount = 1U, .pScissors = nullptr},

        .input_assembly_info {.topology = vk::PrimitiveTopology::eTriangleList,
                              .primitiveRestartEnable = vk::False},

        .rasterization_info {.depthClampEnable = vk::False,    // clamps z to 0-1
                             .rasterizerDiscardEnable = vk::False,
                             .polygonMode = vk::PolygonMode::eFill,
                             .cullMode = vk::CullModeFlagBits::eNone,
                             .frontFace = vk::FrontFace::eClockwise,
                             .depthBiasEnable = vk::False,
                             .depthBiasConstantFactor = 0.0f,    // optional
                             .depthBiasClamp = 0.0f,             // optional
                             .depthBiasSlopeFactor = 0.0f,       // optional
                             .lineWidth = 1.0f},

        .multisample_info {.rasterizationSamples = vk::SampleCountFlagBits::e1,
                           .sampleShadingEnable = vk::False,
                           .minSampleShading = 1.f,
                           .pSampleMask = nullptr,
                           .alphaToCoverageEnable = vk::False,
                           .alphaToOneEnable = vk::False},

        .colourblend_attachment {colourblend_attachment_state},

        .colourblend_info {.logicOpEnable = vk::False,
                           .logicOp = vk::LogicOp::eCopy,
                           .attachmentCount = 1U,
                           .pAttachments = &colourblend_attachment_state,    // TODO: Fix this, on
                                                                             // copy this would
                                                                             // point to old,
                                                                             // deallocated memory
                           .blendConstants = {{0.0f, 0.0f, 0.0f, 0.0f}}},

        .depth_stencil_info {.depthTestEnable = vk::True,
                             .depthWriteEnable = vk::True,
                             .depthCompareOp = vk::CompareOp::eLess,
                             .depthBoundsTestEnable = vk::False,
                             .stencilTestEnable = vk::False,
                             .front = vk::StencilOpState {},
                             .back = vk::StencilOpState {},
                             .minDepthBounds = 0.0f,
                             .maxDepthBounds = 1.0f},
        .dynamic_state_info = {
            .dynamicStateCount = dynamic_states_enabled.size(),
            .pDynamicStates = dynamic_states_enabled.data(),
        }};

    return config_info;
}

auto pipeline::create_pipeline(const pipeline_config_info& config) -> vk::raii::Pipeline {
    const std::array<vk::PipelineShaderStageCreateInfo, 2U> shader_stages {
        vk::PipelineShaderStageCreateInfo {.flags = {},
                                           .stage = vk::ShaderStageFlagBits::eVertex,
                                           .module = vertex_shader_module_,
                                           .pName = "main"},
        vk::PipelineShaderStageCreateInfo {.flags = {},
                                           .stage = vk::ShaderStageFlagBits::eFragment,
                                           .module = fragment_shader_module_,
                                           .pName = "main"}};

    std::vector<vk::VertexInputAttributeDescription> attribute_descriptions = {
        components::vertex::get_attribute_description()};
    std::vector<vk::VertexInputBindingDescription> binding_description = {
        components::vertex::get_binding_description()};

    vk::PipelineVertexInputStateCreateInfo vtx_input_create_info {
        .vertexBindingDescriptionCount = static_cast<uint32_t>(binding_description.size()),
        .pVertexBindingDescriptions = binding_description.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
        .pVertexAttributeDescriptions = attribute_descriptions.data()};

    vk::GraphicsPipelineCreateInfo pipeline_create_info {
        .flags = {},
        .stageCount = shader_stages.size(),
        .pStages = shader_stages.data(),
        .pVertexInputState = &vtx_input_create_info,
        .pInputAssemblyState = &config_.input_assembly_info,
        .pTessellationState = nullptr,
        .pViewportState = &config_.viewport_info,
        .pRasterizationState = &config_.rasterization_info,
        .pMultisampleState = &config_.multisample_info,
        .pDepthStencilState = &config_.depth_stencil_info,
        .pColorBlendState = &config_.colourblend_info,
        .pDynamicState = &config_.dynamic_state_info,
        .layout = config_.pipeline_layout,
        .renderPass = config_.render_pass,
        .subpass = {},
        .basePipelineHandle = nullptr,
        .basePipelineIndex = -1};
    return vk::raii::Pipeline {driver_.device(), nullptr, pipeline_create_info};
}

auto pipeline::create_shader_module(const std::vector<char>& shader_code)
    -> vk::raii::ShaderModule {
    const std::vector<uint32_t> shader_code_converted =
        io::shader_loader::shader_byte_to_u32(shader_code);

    vk::ShaderModuleCreateInfo shader_module_create_info {
        .flags {}, .codeSize = shader_code.size(), .pCode = shader_code_converted.data()};
    return vk::raii::ShaderModule {driver_.device(), shader_module_create_info};
}
}
