#version 330 core

out vec4 FragColor;

in vec3 newColor;

void main()
{
    FragColor = vec4(newColor.x, newColor.y, newColor.z, 1.0f);
}