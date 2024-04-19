#version 450 core

in vec4 Color;
out vec4 FragColor;
in float darknessFactor;


void main()
{
    FragColor = vec4(Color.rgb * darknessFactor, Color.a);
}
