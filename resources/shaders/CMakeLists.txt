include(glsl.cmake)

set(VERTEX_SH_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vertex")
set(FRAGMENT_SH_PATH "${CMAKE_CURRENT_SOURCE_DIR}/fragment")

set(VERTEX_SHADERS "${VERTEX_SH_PATH}/vertex_shader.vert.glsl")

set(FRAGMENT_SHADERS "${FRAGMENT_SH_PATH}/fragment_shader.frag.glsl")

set(SHADER_LIST "${VERTEX_SHADERS} ${FRAGMENT_SHADERS}")
separate_arguments(SHADER_LIST)

foreach(SHADER ${SHADER_LIST})
    set(COMPILED_SHADER "")
    glsl_compile(${SHADER} COMPILED_SHADER)
    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})
endforeach(SHADER)

add_custom_target("shaders"
    DEPENDS ${COMPILED_SHADERS}
    COMMENT "Compiling Shaders"
)
