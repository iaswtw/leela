#version 450 core

in vec4 Color;
in vec2 TexCoord;
in float darknessFactor;

uniform bool useTexture = false;
uniform bool useTexture2 = false;
uniform sampler2D texture1;
uniform sampler2D texture2;

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
