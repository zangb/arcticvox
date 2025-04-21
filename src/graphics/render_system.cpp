#include <vulkan/vulkan_raii.hpp>

#include <glm/matrix.hpp>

#include "arcticvox/components/gameobject.hpp"
#include "arcticvox/components/push_constant.hpp"
#include "arcticvox/graphics/camera.hpp"
#include "arcticvox/graphics/driver.hpp"
#include "arcticvox/graphics/gpu.hpp"
#include "arcticvox/graphics/pipeline.hpp"
#include "arcticvox/graphics/render_system.hpp"

namespace arcticvox::graphics {

render_system::render_system(gpu& gpu, gpu_driver& driver, vk::raii::RenderPass& renderpass) :
    gpu_(gpu),
    driver_(driver),
    pipeline_layout_(create_pipeline_layout()),
    pipeline_(create_pipeline(renderpass)) { }

vk::raii::PipelineLayout render_system::create_pipeline_layout() {
    vk::PushConstantRange pushconstant_range {.stageFlags = vk::ShaderStageFlagBits::eVertex
                                                            | vk::ShaderStageFlagBits::eFragment,
                                              .offset = 0U,
                                              .size = sizeof(components::push_constant_data)};
    vk::PipelineLayoutCreateInfo pipeline_layout_info {.setLayoutCount = 0U,
                                                       .pSetLayouts = nullptr,
                                                       .pushConstantRangeCount = 1U,
                                                       .pPushConstantRanges = &pushconstant_range};
    return vk::raii::PipelineLayout {driver_.device(), pipeline_layout_info};
}

std::unique_ptr<pipeline> render_system::create_pipeline(vk::raii::RenderPass& renderpass) {
    pipeline_config_info pipeline_config = pipeline::get_default_pipeline_config();

    pipeline_config.render_pass = *renderpass;
    pipeline_config.pipeline_layout = *pipeline_layout_;
    return std::make_unique<pipeline>(gpu_, driver_, vtx_shader_, fgt_shader_, pipeline_config);
}

void render_system::render_gameobjects(vk::raii::CommandBuffer& command_buffer,
                                       std::vector<components::gameobject>& gameobjects,
                                       camera& cam) {
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_->vk_pipeline());

    const glm::mat4 projection_view = cam.projection_matrix() * cam.view_matrix();

    for(components::gameobject& obj: gameobjects) {
        components::push_constant_data push_data {
            .transform = projection_view * obj.transform.mat4(),
            .colour = obj.colour,
        };
        command_buffer.pushConstants<components::push_constant_data>(
            *pipeline_layout_,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0U,
            push_data);
        obj.model->bind(command_buffer);
        obj.model->draw(command_buffer);
    }
}
}
