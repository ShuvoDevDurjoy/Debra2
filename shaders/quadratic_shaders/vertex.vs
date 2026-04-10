#version 330 core

layout (location = 0) in vec3 point;
layout (location = 1) in vec3 prev_point;
layout (location = 2) in vec3 next_point;
layout (location = 3) in vec3 unit_normal;
layout (location = 4) in float stroke_width;
layout (location = 5) in vec3 color;
layout (location = 6) in float opacity;

out VS_OUT {
    vec3 point;
    vec3 prev_point;
    vec3 next_point;
    vec3 unit_normal;
    float stroke_width;
    vec4 color;
} vs_out;

void main()
{
    vs_out.point = point;
    vs_out.prev_point = prev_point;
    vs_out.next_point = next_point;
    vs_out.unit_normal = unit_normal;
    vs_out.stroke_width = 20;
    vs_out.color = vec4(color, opacity);
}
