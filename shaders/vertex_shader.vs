// #version 330 core
// layout (location = 0) in vec3 aPos;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// void main()
// {
//     vec4 worldPos = model * vec4(aPos, 1.0);

//     gl_Position = projection * view * worldPos;
// }


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
