#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 ourTexture1;

uniform mat4 view;
uniform mat4 projection;

void main()
{
     ourTexture1 = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}