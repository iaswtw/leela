#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform vec3 offset;

out vec4 Color;

void main()
{
    vec3 offsetPosition = (1 * position) + offset;
    vec3 eye = vec3(1000, 1000, -2000) + offset;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(eye - offsetPosition);

    float ambient = 0.1;
    float diffuse = max(dot(norm, lightDir), 0.0);
    
    
    // apply all 3 transformations to the original point
    gl_Position = projection * vec4(offsetPosition, 1.0);
    Color = (ambient + diffuse) * in_color;
}
