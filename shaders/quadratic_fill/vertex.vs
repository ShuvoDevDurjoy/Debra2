// #version 330 core

// layout (location = 0 ) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
// layout (location = 2) in vec3 normal;

// out vec3 vertexColor;

// out VS_OUT{
//     vec3 pos;
//     int vertex_id;
// }vs_out;

// void main(){
//     vs_out.pos = aPos;
//     vs_out.vertex_id = gl_VertexID;
// }

#version 330 core

layout(location = 0) in vec3 position;

flat out int triangleId; // triangle index to geometry shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = vec4(position, 1.0);

    triangleId = gl_VertexID / 3; // each 3 vertices make a triangle
}
