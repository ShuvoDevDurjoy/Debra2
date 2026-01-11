#version 330 core

layout (location = 0) in vec3 currentPos;
layout (location = 1) in vec3 prevPoint;
layout (location = 2) in vec3 nextPoint;
layout (location = 3) in vec3 color;

out VS_OUT{
    vec3 cp;
    vec3 pP;
    vec3 nP;
    vec3 color;
}vs_out;

flat out int vId;

void main()
{
    vs_out.cp = currentPos;
    vs_out.pP = prevPoint;
    vs_out.nP = nextPoint;
    vs_out.color = color;
    vId = gl_VertexID;
}
