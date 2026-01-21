#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float u_line_width;
uniform int vertexCount;
uniform float uProgress;

in VS_OUT {
    vec3 cp;     // current point
    vec3 pP;     // previous point
    vec3 nP;     // next point
    vec3 color;  // color
} gs_in[];

flat in int vId[];

out vec3 outColor;

void main()
{
    float halfW = u_line_width * 0.5;

    // 1. Define segments
    vec3 p0 = gs_in[0].cp;
    vec3 p1 = gs_in[1].cp;
    
    // Directions of current, previous, and next segments
    vec3 dCurr = normalize(p1 - p0);
    vec3 dPrev = (distance(p0, gs_in[0].pP) < 1e-4) ? dCurr : normalize(p0 - gs_in[0].pP);
    vec3 dNext = (distance(p1, gs_in[1].nP) < 1e-4) ? dCurr : normalize(gs_in[1].nP - p1);

    // 2. Calculate Normals (2D perpendiculars in the XY plane of the segment)
    // If you are working in 3D, you'd need a consistent 'up' vector or the plane normal
    vec3 nCurr = vec3(-dCurr.y, dCurr.x, 0.0);
    vec3 nPrev = vec3(-dPrev.y, dPrev.x, 0.0);
    vec3 nNext = vec3(-dNext.y, dNext.x, 0.0);

    // 3. Calculate Miter Vectors (Average of the normals)
    vec3 miter0 = normalize(nPrev + nCurr);
    vec3 miter1 = normalize(nCurr + nNext);

    // 4. Calculate Miter Lengths
    // Length = halfWidth / cos(theta/2) -> which is halfWidth / dot(miter, normal)
    float length0 = halfW / max(dot(miter0, nCurr), 0.5); // 0.5 limit prevents extreme miters
    float length1 = halfW / max(dot(miter1, nCurr), 0.5);

    // 5. Generate Quad Vertices
    // We emit in a Z-pattern: Bottom-Left, Top-Left, Bottom-Right, Top-Right
    vec3 v0 = p0 - miter0 * length0; // Bottom Start
    vec3 v1 = p0 + miter0 * length0; // Top Start
    vec3 v2 = p1 - miter1 * length1; // Bottom End
    vec3 v3 = p1 + miter1 * length1; // Top End

    mat4 mvp = projection * view * model;

    // Emit Vertices
    outColor = gs_in[0].color;
    gl_Position = mvp * vec4(v0, 1.0); EmitVertex();
    gl_Position = mvp * vec4(v1, 1.0); EmitVertex();
    
    outColor = gs_in[1].color;
    gl_Position = mvp * vec4(v2, 1.0); EmitVertex();
    gl_Position = mvp * vec4(v3, 1.0); EmitVertex();

    EndPrimitive();
}