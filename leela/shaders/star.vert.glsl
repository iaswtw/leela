#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform uint starPointSize;

out vec4 Color;

void main()
{
    gl_PointSize = starPointSize;
    gl_Position = proj * view * model * vec4(position, 1.0);
    Color = in_color;
}