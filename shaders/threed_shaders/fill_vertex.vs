#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 u_resolution; 

out vec3 FragPos;
out vec2 v_uv;
out vec3 vNormal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
    
    int T = int(u_resolution.y);
    int R = int(u_resolution.x);
    if (T > 1 && R > 1) {
        int i = gl_VertexID / T;
        int j = gl_VertexID % T;
        float u = float(i) / float(R - 1);
        float v = float(j) / float(T - 1);
        v_uv = vec2(u, v);
    } else {
        v_uv = vec2(1.0, 1.0);
    }
}
