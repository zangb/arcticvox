find_program(GLSL_VALIDATOR glslangValidator)

# Function glsl_compile compiles a list of shaders to the spirv format
# SHADERS: the shader to compile, assumes filenames of type .shader.glsl,
# e.g. .vert.glsl
# COMPILED_SHADERS: the name of the compiled shader
function(glsl_compile SHADER COMPILED_SHADER)
    set(OUTPUT_NAME "")
    cmake_path(REMOVE_EXTENSION SHADER LAST_ONLY OUTPUT_VARIABLE OUTPUT_NAME)
    cmake_path(GET OUTPUT_NAME FILENAME OUTPUT_NAME)

    set(SPIRV "${CMAKE_BINARY_DIR}/shaders/${OUTPUT_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSL_VALIDATOR} -V ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER}"
    )
    set(COMPILED_SHADER ${SPIRV} PARENT_SCOPE)
endfunction()
