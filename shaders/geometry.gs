layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform float pointSize;

void main() {
    vec4 P = gl_in[0].gl_Position;

    gl_Position = P + vec4(-pointSize, -pointSize, 0, 0);
    EmitVertex();

    gl_Position = P + vec4( pointSize, -pointSize, 0, 0);
    EmitVertex();

    gl_Position = P + vec4(-pointSize,  pointSize, 0, 0);
    EmitVertex();

    gl_Position = P + vec4( pointSize,  pointSize, 0, 0);
    EmitVertex();

    EndPrimitive();
}
