#version 450

layout(location = 0) in vec3 frag_colour;
/* to configure via the pipeline */
layout(location = 0) out vec4 colour_out;

layout(push_constant) uniform push_data {
    mat4 transform;
    vec3 colour;
}
push;

void main() {
    colour_out = vec4(frag_colour, 1.0);
}
