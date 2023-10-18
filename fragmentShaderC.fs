#version 330 core
out vec4 FragColor;

in vec4 Color;
in vec2 TexCoords;
in float UseTexture;

uniform sampler2D texture_diffuse1;

void main()
{
    if (int(UseTexture) == 0)
    {
        FragColor = Color;
    }
    else
    {
        FragColor = texture(texture_diffuse1, TexCoords);
    }
}