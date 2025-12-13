#version 330 core

layout(location = 0) in vec3 aPos;    // vertex position

out vec3 vPos;  // pass to geometry shader

void main()
{
    vPos = aPos;
}
