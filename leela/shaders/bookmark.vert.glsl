#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform vec3 offset;

out vec4 Color;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    vec3 offsetPosition = (1 * position) + offset;

    // apply all 3 transformations to the original point
    gl_Position = projection * vec4(offsetPosition, 1.0);
    FragPos = offsetPosition;
    Normal = normal;
    //Normal = mat3(transpose(inverse(model))) * aNormal;  
}
