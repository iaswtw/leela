#version 150 core

in vec4 Color;

out vec4 outColor;

void main()
{
    // outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    outColor = Color;
}