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

set(GRAPHICS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/graphics_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/window.cpp")

set(IO_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/io/cursor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/io/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/io/shaderloader.cpp")

set(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# create actual arcticvox executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${IO_SOURCE_FILES} ${GRAPHICS_SOURCE_FILES})

# add a dependency to update the symbolic link to the compile commands based on the current
# build mode / preset
add_dependencies(${PROJECT_NAME} compile_commands_symlink)

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PRIVATE
    vulkan
    glfw
    ${IMGUI}
    spdlog::spdlog
    EnTT::EnTT)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    VULKAN_HPP_NO_CONSTRUCTORS
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_ENABLE_EXPERIMENTAL)

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})
