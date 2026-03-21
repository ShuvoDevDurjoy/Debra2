#version 330 core
out vec4 FragColor;

in vec2 g_uv;

uniform vec3 strokeColor = vec3(0.5, 0.5, 1.0);
uniform float u_progress = 1.0;
uniform int u_generation_direction = 0;

void main()
{
    float val = (u_generation_direction == 0) ? g_uv.y : g_uv.x;
    if (val > u_progress) discard;
    
    FragColor = vec4(strokeColor, 1.0);
}
