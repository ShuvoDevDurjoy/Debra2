#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 FragPos[];
in vec2 v_uv[];

out vec2 g_uv;

uniform float u_stroke_width = 3.0; // In clip-space coordinate scale (roughly thickness at depth=1)
uniform vec2 u_screen_size = vec2(1200.0, 600.0);

void main() {
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;

    // Convert to screen coordinates
    vec2 pos0 = p0.xy / p0.w;
    vec2 pos1 = p1.xy / p1.w;
    
    vec2 dir = pos1 - pos0;
    if (length(dir) > 0.0) dir = normalize(dir);
    else dir = vec2(1.0, 0.0);
    
    vec2 normal = vec2(-dir.y, dir.x);
    
    // Scale normal to screen space pixels
    vec2 offset = normal * (u_stroke_width / u_screen_size);

    // Scale to reference depth (e.g. 10.0) so width scales with zoom
    vec4 offset0 = vec4(offset * 10.0, 0.0, 0.0);
    vec4 offset1 = vec4(offset * 10.0, 0.0, 0.0);

    gl_Position = p0 + offset0;
    g_uv = v_uv[0];
    EmitVertex();

    gl_Position = p0 - offset0;
    g_uv = v_uv[0];
    EmitVertex();

    gl_Position = p1 + offset1;
    g_uv = v_uv[1];
    EmitVertex();

    gl_Position = p1 - offset1;
    g_uv = v_uv[1];
    EmitVertex();

    EndPrimitive();
}
