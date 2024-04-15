#version 330 core


layout(location = 0) in vec3 vertex;   // <vec3 pos>
layout(location = 1) in vec2 tex;      // <vec2 tex>

out vec2 TexCoords;

uniform mat4 projection;


void main()
{
    gl_Position = projection * vec4(vertex, 1.0);
    TexCoords = tex;
}


//layout (location = 0) in vec4 vertex;   // <vec2 pos, vec2 tex>
//out vec2 TexCoords;
//
//uniform mat4 projection;
//
//
//void main()
//{
//    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
//    TexCoords = vertex.zw;
//}
