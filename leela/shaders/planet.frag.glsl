#version 450 core

in vec4 Color;
in vec2 TexCoord;
in float darknessFactor;

layout (location = 0) uniform bool useTexture = false;
layout (location = 1) uniform bool useTexture2 = false;
layout (location = 2) uniform sampler2D texture1;
layout (location = 3) uniform sampler2D texture2;

out vec4 FragColor;

void main()
{
    if (useTexture) {
        FragColor = texture(texture1, TexCoord) * darknessFactor;
        if (useTexture2) {
            FragColor += texture(texture2, TexCoord) * darknessFactor;
        }
    }
    else {
        FragColor = vec4(Color.rgb * darknessFactor, Color.a);
    }
}
