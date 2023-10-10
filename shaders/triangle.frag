#version 450

layout (location = 0) out vec4 glFragColor;

layout (location = 0) in vec4 in_color;

void main()  {
    glFragColor = in_color;
}
