#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float u_fill_progress;
uniform int vertexCount;

flat in int vId[]; 
// Note: We need vId from the vertex shader to know the indices of the triangle's points.

void main() {
    if (u_fill_progress <= 0.0) return;
    if (u_fill_progress >= 1.0) {
        // Emit the triangle unchanged
        gl_Position = gl_in[0].gl_Position; EmitVertex();
        gl_Position = gl_in[1].gl_Position; EmitVertex();
        gl_Position = gl_in[2].gl_Position; EmitVertex();
        EndPrimitive();
        return;
    }

    // A GL_TRIANGLE_FAN emits triangles: (0, 1, 2), (0, 2, 3), (0, 3, 4)...
    // The sweep progress corresponds to the 2nd and 3rd vertices.
    // Progress maps [0, 1] to [0, vertexCount-1].
    float targetIndex = u_fill_progress * float(vertexCount - 1);

    // vId[1] is the generating edge start, vId[2] is the edge end.
    float idx1 = float(vId[1]);
    float idx2 = float(vId[2]);

    // If the whole triangle is before the sweep line, emit it fully.
    if (idx2 <= targetIndex) {
        gl_Position = gl_in[0].gl_Position; EmitVertex();
        gl_Position = gl_in[1].gl_Position; EmitVertex();
        gl_Position = gl_in[2].gl_Position; EmitVertex();
        EndPrimitive();
    }
    // If the sweep line cuts through this triangle, clip it.
    else if (idx1 < targetIndex && idx2 > targetIndex) {
        float t = (targetIndex - idx1) / (idx2 - idx1);
        
        vec4 clipped_pos = mix(gl_in[1].gl_Position, gl_in[2].gl_Position, t);
        
        gl_Position = gl_in[0].gl_Position; EmitVertex();
        gl_Position = gl_in[1].gl_Position; EmitVertex();
        gl_Position = clipped_pos; EmitVertex();
        EndPrimitive();
    }
    // Else, the triangle is completely after the sweep line, emit nothing.
}
