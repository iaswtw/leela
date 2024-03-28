#version 330 core

// layout (location = 0) in vec3 position;
// layout (location = 1) in vec4 in_color;
// layout (location = 2) in vec2 texCoord;
// 
// out vec2 TexCoord;
// 
// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 proj;
// 
// 
// 
// void main()
// {
//     gl_Position = proj * view * model * vec4(position, 1.0);
//     TexCoord = texCoord;
// }


layout (location = 0) in vec4 vertex;   // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;


void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}