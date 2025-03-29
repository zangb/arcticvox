#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;

layout(location = 0) out vec3 frag_colour;

layout(push_constant) uniform push_data {
    mat4 transform;
    vec3 colour;
}
push;

void main() {
    // gl_VertexIndex: To index into the array, contains the current vertex for
    // each vertex the main function is run
    // z = 0.0 it's at the front
    // alpha = whole vector is divided by it
    gl_Position = push.transform * vec4(position, 1.0);
    frag_colour = colour;
}
