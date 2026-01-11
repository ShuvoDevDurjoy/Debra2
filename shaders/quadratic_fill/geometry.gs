// #version 330 core

// layout (triangles) in;
// layout (triangle_strip, max_vertices = 3) out;

// uniform float uProgress;        // 0.0 -> 1.0 overall progress
// uniform int vertexCount;        // total number of vertices in the mesh
// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// in VS_OUT{
//     vec3 pos;
//     int vertex_id;
// }gs_in[];

// // Linear interpolation between two points
// vec3 lerp(vec3 a, vec3 b, float t) {
//     return a + t * (b - a);
// }

// void main() {
//     if (uProgress <= 0.0) return;

//     // Each triangle has 3 vertices
//     vec3 p0 = gs_in[0].pos;
//     vec3 p1 = gs_in[1].pos;
//     vec3 p2 = gs_in[2].pos;

//     // Map each vertex to a normalized "progress" [0,1] based on its index
//     // Assuming vertexCount is total vertices of the mesh in order
//     float v0Prog = float(gs_in[0].vertex_id) / float(vertexCount - 1);
//     float v1Prog = float(gs_in[1].vertex_id) / float(vertexCount - 1);
//     float v2Prog = float(gs_in[2].vertex_id) / float(vertexCount - 1);

//     // If entire triangle is beyond uProgress, skip it
//     if (uProgress < v0Prog) return;

//     // Compute how much of the triangle to emit
//     float t0 = clamp((uProgress - v0Prog) / max(0.0001, v1Prog - v0Prog), 0.0, 1.0);
//     float t1 = clamp((uProgress - v1Prog) / max(0.0001, v2Prog - v1Prog), 0.0, 1.0);
//     float t2 = clamp((uProgress - v2Prog) / max(0.0001, 1.0 - v2Prog), 0.0, 1.0);

//     // Interpolate vertices toward next ones based on progress
//     vec3 ip0 = lerp(p0, p1, t0);
//     vec3 ip1 = lerp(p1, p2, t1);
//     vec3 ip2 = lerp(p2, p0, t2);

//     // Emit the triangle
//     gl_Position = projection * view * model * vec4(ip0, 1.0);
//     EmitVertex();

//     gl_Position = projection * view * model * vec4(ip1, 1.0);
//     EmitVertex();

//     gl_Position = projection * view * model * vec4(ip2, 1.0);
//     EmitVertex();

//     EndPrimitive();
// }


#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform float uProgress;    // global progress 0.0 → 1.0
uniform int vertexCount;    // total number of vertices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float pProgress;
out float triProgress;

out vec3 FragPos;
out vec3 Normal;
flat out int vertexId;

flat in int triangleId[];   // triangle ID from vertex shader

// Linear interpolation helper
vec3 lerp(vec3 a, vec3 b, float t) {
    return a + t * (b - a);
}

void main() {
    if (uProgress <= 0.0) return;

    // Triangle vertices
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;

    Normal = normalize(cross(p1 - p0, p2 - p0));

    // Determine total triangles
    float totalTriangles = float(vertexCount) / 3.0;

    // Compute triangle's start and end progress
    float startProg = float(triangleId[0]) / totalTriangles;
    float endProg   = float(triangleId[0] + 1) / totalTriangles;

    // Compute progress for this triangle
    triProgress = clamp((uProgress - startProg) / (endProg - startProg), 0.0, 1.0);

    // If this triangle hasn't started yet, skip
    if (triProgress <= 0.0) return;

    // Optional: interpolate triangle vertices to animate growth (can be commented out)
    vec3 ip0 = p0;
    vec3 ip1 = p1;
    vec3 ip2 = p2;
    // Emit triangle
    FragPos = ip0;
    pProgress = 1;
    gl_Position = projection * view * model * vec4(ip0, 1.0);
    EmitVertex();

    FragPos = ip1;
    pProgress = 0;
    gl_Position = projection * view * model * vec4(ip1, 1.0);
    EmitVertex();

    FragPos = ip2;
    pProgress = 1;
    gl_Position = projection * view * model * vec4(ip2, 1.0);
    EmitVertex();

    EndPrimitive();
}
