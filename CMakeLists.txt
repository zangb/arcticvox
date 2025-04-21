cmake_minimum_required(VERSION 3.28)

project(arcticvox VERSION 0.0.1)

# Set up dependencies
add_subdirectory(external)
add_subdirectory(resources)

# Set custom compile flags
set(COMPILE_FLAGS "-Wall" "-Werror" "-Wpedantic" "-std=c++20")

enable_testing()

add_custom_target(
    compile_commands_symlink
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ln -snf ${CMAKE_COMPILE_COMMANDS} "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
    COMMENT "Create symlink to compile_commands.json"
)

set(COMPONENTS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/components/fps_camera_controller.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/components/model.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/components/vertex.cpp")

set(GRAPHICS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/driver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/gpu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/pipeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/render_system.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/swapchain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/window.cpp")

set(IO_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/io/input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/io/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/io/model_builder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/io/shaderloader.cpp")

set(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# create actual arcticvox executable
add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${COMPONENTS_SOURCE_FILES}
    ${GRAPHICS_SOURCE_FILES}
    ${IO_SOURCE_FILES})

# add a dependency to update the symbolic link to the compile commands
# based on the current build mode / preset
add_dependencies(${PROJECT_NAME} compile_commands_symlink)
add_dependencies(${PROJECT_NAME} "shaders")
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PRIVATE
    vulkan
    glfw
    ${IMGUI}
    spdlog::spdlog
    EnTT::EnTT
    assimp::assimp)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    VULKAN_HPP_NO_CONSTRUCTORS
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_ENABLE_EXPERIMENTAL)

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})
