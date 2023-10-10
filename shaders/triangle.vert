#version 450

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_normal;
layout (location = 2) in vec4 vertex_color;
layout (location = 3) in vec2 uv_coords;

layout (location = 0) out vec4 out_color;

layout (push_constant) uniform constants {
    mat4x4 transform;
} push_constants;


void main()  {
    vec4 transformed_pos = push_constants.transform * vertex_position;
    vec4 transformed_norm = push_constants.transform * vertex_normal;

    vec3 out_pos = transformed_pos.xyz / transformed_pos.w;
    out_pos.y = -out_pos.y;
    gl_Position = vec4(out_pos, 1.f);

    out_color = vec4(abs(transformed_norm.xyz), 1.f);
    //out_color = vec4(abs(uv_coords), 0.f, 1.f);
}
