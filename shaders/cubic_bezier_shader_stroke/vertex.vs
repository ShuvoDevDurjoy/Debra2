#version 330 core

layout (location = 0) in vec3 currentPosition;
layout (location = 1) in vec3 prevPosition;
layout (location = 2) in vec3 nextPosition;

out VS_OUT{
    vec3 cP;
    vec3 pP;
    vec3 nP;
}vs_out;

void main(){
    vs_out.cP = currentPosition;
    vs_out.pP = prevPosition;
    vs_out.nP = nextPosition;
}