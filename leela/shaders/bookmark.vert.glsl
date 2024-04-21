#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 model;             // for the purpose of lighting. transform to world coordinates.
uniform mat4 drawModel;         // bookmark is drawn 
uniform vec3 S;

out vec4 Color;
out float darknessFactor;

void main()
{
    vec3 model_x_position       = vec3(model * vec4(position, 1.0));
    //vec3 model_x_normal         = vec3(model * vec4(normal, 1.0));
    vec3 model_x_normal = normal;
    
    vec3 x_normal               = normalize(model_x_normal);       // transformed normal of the current vertex
    vec3 lightDir               = normalize(model_x_position - S);
    
    float dotProduct            = dot(lightDir, x_normal);
    
    float diffuse   = max(dotProduct, 0.0);
    
    darknessFactor = diffuse;
    
    // apply all 3 transformations to the original point
    //gl_Position = projection * drawModel * vec4(position, 1.0);
    gl_Position = projection * drawModel * vec4(position, 1.0);
    Color = in_color;
}
