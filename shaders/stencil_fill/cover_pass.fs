#version 330 core

out vec4 FragColor;

uniform vec3  u_fill_color   = vec3(1.0);
uniform float u_fill_opacity = 1.0;
uniform float u_fill_progress = 1.0; // 0..1

void main() {
    float alpha = u_fill_opacity;
    // Simple logic: if progress is 1.0, show fully. 
    // If we wanted a more complex reveal, we'd need vertex params.
    // For now, we'll use a simple threshold or just scale opacity.
    if (u_fill_progress < 0.01) discard;
    
    FragColor = vec4(u_fill_color, alpha * u_fill_progress);
}
