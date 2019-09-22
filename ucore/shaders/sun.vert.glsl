#version 330 core

in vec3 position;
in vec4 in_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 Color;

void main()
{
    gl_Position = proj * view * model * vec4(position, 1.0);
}