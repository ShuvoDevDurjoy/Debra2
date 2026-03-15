#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float uProgress;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float u_line_width;
uniform vec2 uViewportSize;
uniform int vertexCount;
uniform int u_layer;
uniform float uMiterLimit = 2.0;
uniform int uStrokeJoinStyle = 0;  // 0=MITER, 1=BEVAL, 2=ROUND

out vec2 perpControl;
out vec2 prepStart;
out vec2 prevEnd;
out vec2 pCurrent;
out float pProgress;
out float progress;
out float stroke_width;
flat out int vertexId;
out vec3 outColor;

uniform float user_bezier_always = 0.0;

in VS_OUT{
    vec3 cp;
    vec3 pP;
    vec3 nP;
    vec3 color;
} gs_in[];

flat in int vId[];

vec3 norm3(vec3 v){
    float len = length(v);
    if(len < 1e-3) return vec3(0,0,0);
    return v / len;
}

vec2 norm2(vec2 v){
    float len = length(v);
    if(len < 1e-3) return vec2(0,0);
    return v / len;
}

void main()
{
    if(uProgress <= 0.01) return;

    float prevProg = float(vId[0]) / float(vertexCount - 1);
    float nextProg = float(vId[1]) / float(vertexCount - 1);
    if(uProgress <= prevProg) return;
    progress = clamp((uProgress - prevProg)/(nextProg - prevProg), 0.0, 1.0);
    
    // 1. Calculate directions in Object Space
    vec3 v0 = gs_in[0].cp;
    vec3 v1 = gs_in[1].cp;
    vec3 vP = gs_in[0].pP;
    vec3 vN = gs_in[1].nP;

    bool isStart = length(gs_in[0].cp - gs_in[0].pP) < 1e-5;
    bool isEnd   = length(gs_in[1].cp - gs_in[1].nP) < 1e-5;

    vec3 d1 = norm3(v1 - v0);
    vec3 d0 = isStart ? d1 : norm3(v0 - vP);
    vec3 d2 = isEnd   ? d1 : norm3(vN - v1);

    // 2. Perpendiculars in Object Space (assuming XY plane)
    vec3 perp0 = vec3(-d0.y, d0.x, 0.0);
    vec3 perp1 = vec3(-d1.y, d1.x, 0.0);
    vec3 perp2 = vec3(-d2.y, d2.x, 0.0);

    // 3. Miter logic in Object Space
    vec3 miterStart = isStart ? perp1 : norm3(perp0 + perp1);
    vec3 miterEnd   = isEnd   ? perp1 : norm3(perp1 + perp2);
    
    float miterScaleStart = isStart ? 1.0 : 1.0 / max(abs(dot(miterStart, perp1)), 0.25);
    float miterScaleEnd   = isEnd   ? 1.0 : 1.0 / max(abs(dot(miterEnd, perp1)), 0.25);
    
    miterScaleStart = min(miterScaleStart, uMiterLimit);
    miterScaleEnd   = min(miterScaleEnd,   uMiterLimit);

    float halfW = u_line_width * 0.5;
    
    // We'll use a fixed object-space width for the "paper" look.
    // If you want it to behave like the old version (pixel-constant), 
    // you'd need to scale halfW by a distance factor, but let's keep it 
    // strictly object-space for true "sheet of paper" behavior.
    float objHalfW = halfW * 0.1; // Scale down for object space units

    // 4. Bezier/Miter expansion in Object Space
    vec3 offsetStart = miterStart * (objHalfW * miterScaleStart);
    vec3 offsetEnd   = miterEnd   * (objHalfW * miterScaleEnd);

    vec3 p0_obj = v0 - offsetStart;
    vec3 p1_obj = v0 + offsetStart;
    vec3 p2_obj = v1 - offsetEnd;
    vec3 p3_obj = v1 + offsetEnd;

    // Project to Clip Space
    mat4 pvm = projection * view * model;

    // Bezier control logic (pCurrent etc)
    float segLen = length(v1 - v0);
    prepStart = vec2(0.0, 0.0);
    prevEnd = vec2(segLen, 0.0);
    stroke_width = objHalfW;
    perpControl = vec2(segLen * 0.5, 0.0); // Simplified flat control for now

    // Emit vertices
    // Apply a tiny Z-offset based on layer and a stroke bias to prevent Z-fighting
    float zBias = float(u_layer) * 0.001 + 0.0001; 

    // p0
    vec3 local = p0_obj - v0;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 0.0; outColor = gs_in[0].color;
    gl_Position = pvm * vec4(p0_obj, 1.0);
    gl_Position.z -= zBias * gl_Position.w; // Push forward in NDC
    EmitVertex();

    // p1
    local = p1_obj - v0;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 0.0; outColor = gs_in[0].color;
    gl_Position = pvm * vec4(p1_obj, 1.0);
    gl_Position.z -= zBias * gl_Position.w;
    EmitVertex();

    // p2
    local = p2_obj - v0;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 1.0; outColor = gs_in[1].color;
    gl_Position = pvm * vec4(p2_obj, 1.0);
    gl_Position.z -= zBias * gl_Position.w;
    EmitVertex();

    // p3
    local = p3_obj - v0;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 1.0; outColor = gs_in[1].color;
    gl_Position = pvm * vec4(p3_obj, 1.0);
    gl_Position.z -= zBias * gl_Position.w;
    EmitVertex();

    EndPrimitive();
}
