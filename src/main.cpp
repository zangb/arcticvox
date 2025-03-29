#include <spdlog/spdlog.h>

#include "arcticvox/graphics/graphics_device.hpp"
#include "arcticvox/graphics/window.hpp"

auto main(int argc, char** argv) -> int {
    try {
        arcticvox::graphics::window window {1240, 720, "Arctic Vox"};
        arcticvox::graphics::graphics_device device {"Arctic Vox", 0, window};
    } catch(const std::exception& e) {
    }
}
