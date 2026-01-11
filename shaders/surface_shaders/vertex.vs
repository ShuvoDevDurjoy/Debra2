#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;


out vec3 FragPos;
out vec3 realPos;
out vec3 Normal;

flat out int vertexId;

out VS_OUT{
    vec3 pos;
    vec3 color;
}vs_out;

void main()
{
    vs_out.pos = aPos;
    vs_out.color = aColor;
    vertexId = gl_VertexID;
}
