add_subdirectory(entt)
add_subdirectory(spdlog)

# create IMGUI library from raw sources
set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
)

set(IMGUI "imgui")
add_library(${IMGUI} STATIC ${IMGUI_SOURCES})
target_include_directories(${IMGUI} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/imgui>)
target_compile_options(${IMGUI} PRIVATE ${COMPILE_FLAGS})

