#include <csignal>
#include <vector>

#include <spdlog/spdlog.h>

#include "arcticvox/components/fps_camera_controller.hpp"
#include "arcticvox/components/gameobject.hpp"
#include "arcticvox/components/model.hpp"
#include "arcticvox/graphics/camera.hpp"
#include "arcticvox/graphics/engine.hpp"

std::vector<arcticvox::components::gameobject> load_gameobjects(
    arcticvox::graphics::gpu_driver& driver) {
    std::vector<arcticvox::components::gameobject> objs {};

    arcticvox::io::model_builder builder {};
    if(!builder.load_model("/home/fubutea/repos/HoloVox/resources/models/2b_kimono/source/28.glb"))
        throw std::runtime_error("Unable to open model file");

    auto model = std::make_shared<arcticvox::components::model>(driver, builder);

    auto gameobj = arcticvox::components::gameobject::make_gameobject();
    gameobj.model = model;
    gameobj.transform.rotation =
        glm::angleAxis(-glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f))
        * glm::angleAxis(-glm::half_pi<float>(), glm::vec3(0.0f, 0.0, 1.0f));
    gameobj.transform.translation = {0.0f, 0.0f, 2.5f};
    gameobj.transform.scale = {2., 2.f, 2.f};
    objs.push_back(std::move(gameobj));

    return objs;
}

auto main(int argc, char** argv) -> int {
    arcticvox::graphics::graphics_engine avox_engine {1280, 720, "Arctic Vox"};
    arcticvox::graphics::camera camera {};
    arcticvox::components::fps_camera_controller cam_controller {avox_engine.get_window()};
    std::vector<arcticvox::components::gameobject> render_objects =
        load_gameobjects(avox_engine.get_gpu_driver());
    camera.set_view_direction(glm::vec3(0.0f), glm::vec3(0.f, 0.0f, 1.f));
    try {
        camera.set_camera_controller(cam_controller);
        avox_engine.set_camera(camera);
        avox_engine.set_objects_to_render(render_objects);
        avox_engine.run();
    } catch(const std::exception& e) {
        spdlog::error(e.what());
    }
    return 0;
}
