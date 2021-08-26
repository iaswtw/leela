#version 330 core

in vec4 Color;
in vec2 TexCoord;
in float darknessFactor;

uniform bool useTexture = false;
uniform sampler2D ourTexture;

out vec4 FragColor;

void main()
{
    if (useTexture) {
        FragColor = texture(ourTexture, TexCoord) * darknessFactor;
    }
    else {
        FragColor = vec4(Color.rgb * darknessFactor, Color.a);
    }
}
