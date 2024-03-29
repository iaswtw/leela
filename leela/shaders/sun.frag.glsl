#version 330 core

in vec4 Color;
in vec2 TexCoord;

uniform bool useTexture = false;
uniform sampler2D outTexture;

out vec4 FragColor;

void main()
{
    if (useTexture) {
        FragColor = texture(outTexture, TexCoord);
    }
    else {
        FragColor = Color;
    }

}
