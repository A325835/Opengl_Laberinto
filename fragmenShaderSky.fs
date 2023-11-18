#version 330 core
out vec4 FragColor;

in vec3 ourTexture1;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, ourTexture1);
}