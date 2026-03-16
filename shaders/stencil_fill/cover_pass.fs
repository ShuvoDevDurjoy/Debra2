#version 330 core

out vec4 FragColor;

uniform vec3  u_fill_color   = vec3(1.0);
uniform float u_fill_opacity = 1.0;
uniform float u_fill_progress = 1.0; // 0..1

void main() {
    float alpha = u_fill_opacity;
    FragColor = vec4(1, 0, 0, alpha);
}
