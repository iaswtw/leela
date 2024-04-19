#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 model;
uniform vec3 S;

out vec4 Color;
out float darknessFactor;

void main()
{
    float daylightShadingMultiplier = 1.0;
    vec3 x_normal   = normalize(vec3(model * vec4(normal, 0.0)));       // transformed normal of the current vertex
    vec4 model_x_position = model * vec4(position, 1.0);
    
    //float dotProduct = dot(normalize(S - vec3(model_x_position)), x_normal);
    //daylightShadingMultiplier = sqrt(min(1.0, dotProduct));
    
    darknessFactor = daylightShadingMultiplier;
    
    // apply all 3 transformations to the original point
    gl_Position = projection * model * vec4(position, 1.0);
    Color = in_color;
}
