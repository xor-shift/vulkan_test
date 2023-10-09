#version 450

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_normal;
layout (location = 2) in vec4 vertex_color;

layout (location = 0) out vec4 out_color;

void main()  {
    gl_Position = vec4(vertex_position.xyz / vertex_position.w, 1.0f);
    out_color = vertex_color;
}
