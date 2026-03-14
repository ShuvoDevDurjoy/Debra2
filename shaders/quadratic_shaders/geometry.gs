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
    
    mat4 pvm = projection * view * model;

    // 1. Project points to Clip Space
    vec4 P0_clip = pvm * vec4(gs_in[0].cp, 1.0);
    vec4 P1_clip = pvm * vec4(gs_in[1].cp, 1.0);
    vec4 PP_clip = pvm * vec4(gs_in[0].pP, 1.0);
    vec4 NP_clip = pvm * vec4(gs_in[1].nP, 1.0);

    // 2. To Pixel Space
    vec2 P0_px = (P0_clip.xy / P0_clip.w * 0.5 + 0.5) * uViewportSize;
    vec2 P1_px = (P1_clip.xy / P1_clip.w * 0.5 + 0.5) * uViewportSize;
    
    bool isStart = length(gs_in[0].cp - gs_in[0].pP) < 1e-5;
    bool isEnd   = length(gs_in[1].cp - gs_in[1].nP) < 1e-5;

    vec2 PP_px = isStart ? P0_px : (PP_clip.xy / PP_clip.w * 0.5 + 0.5) * uViewportSize;
    vec2 NP_px = isEnd   ? P1_px : (NP_clip.xy / NP_clip.w * 0.5 + 0.5) * uViewportSize;

    // 3. Directions in Pixel Space
    vec2 d1 = norm2(P1_px - P0_px);
    vec2 d0 = isStart ? d1 : norm2(P0_px - PP_px);
    vec2 d2 = isEnd   ? d1 : norm2(NP_px - P1_px);

    // 4. Perpendiculars (Rotated 90 deg)
    vec2 perp0 = vec2(-d0.y, d0.x);
    vec2 perp1 = vec2(-d1.y, d1.x);
    vec2 perp2 = vec2(-d2.y, d2.x);

    // 5. Miter logic in Pixel Space
    vec2 miterStart = isStart ? perp1 : norm2(perp0 + perp1);
    vec2 miterEnd   = isEnd   ? perp1 : norm2(perp1 + perp2);
    
    float miterScaleStart = isStart ? 1.0 : 1.0 / max(abs(dot(miterStart, perp1)), 0.25);
    float miterScaleEnd   = isEnd   ? 1.0 : 1.0 / max(abs(dot(miterEnd, perp1)), 0.25);
    
    miterScaleStart = min(miterScaleStart, uMiterLimit);
    miterScaleEnd   = min(miterScaleEnd,   uMiterLimit);

    float halfW = u_line_width * 0.5;
    
    // 6. Bezier Control Points in Pixel Space (to support smooth curves at corners)
    // We reuse the theta logic but in 2D pixel space
    float anglePrev = acos(clamp(dot(d0, d1), -1.0, 1.0));
    float angleNext = acos(clamp(dot(d1, d2), -1.0, 1.0));
    float signPrev = sign(d0.x * d1.y - d0.y * d1.x); // 2D cross product sign
    float signNext = sign(d1.x * d2.y - d1.y * d2.x);
    
    float theta = (0.5 * (anglePrev * signPrev + angleNext * signNext));
    float segLen = length(P1_px - P0_px);
    
    float controlY = 0.0;
    float controlX = segLen * 0.5;
    
    const float SHARP_CORNER_THRESHOLD = radians(11.5);
    if(abs(theta) < SHARP_CORNER_THRESHOLD && !isStart && !isEnd) {
        controlY = tan(theta * 0.5) * segLen * 0.6;
        controlY = clamp(controlY, -halfW * 2.0, halfW * 2.0);
    }

    perpControl = vec2(controlX, controlY);
    prepStart = vec2(0.0, 0.0);
    prevEnd = vec2(segLen, 0.0);
    stroke_width = halfW;

    // 7. Expansion
    float expand = 1.0 + clamp(abs(controlY) / (halfW * 2.0), 0.0, 0.5);
    vec2 offsetStart = miterStart * (halfW * miterScaleStart * expand);
    vec2 offsetEnd   = miterEnd   * (halfW * miterScaleEnd   * expand);

    vec2 p0_px = P0_px - offsetStart;
    vec2 p1_px = P0_px + offsetStart;
    vec2 p2_px = P1_px - offsetEnd;
    vec2 p3_px = P1_px + offsetEnd;

    vec2 local;
    
    // p0
    local = p0_px - P0_px;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 0.0; outColor = gs_in[0].color;
    gl_Position = vec4(((p0_px / uViewportSize - 0.5) * 2.0) * P0_clip.w, P0_clip.z, P0_clip.w);
    EmitVertex();

    // p1
    local = p1_px - P0_px;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 0.0; outColor = gs_in[0].color;
    gl_Position = vec4(((p1_px / uViewportSize - 0.5) * 2.0) * P0_clip.w, P0_clip.z, P0_clip.w);
    EmitVertex();

    // p2
    local = p2_px - P0_px;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 1.0; outColor = gs_in[1].color;
    gl_Position = vec4(((p2_px / uViewportSize - 0.5) * 2.0) * P1_clip.w, P1_clip.z, P1_clip.w);
    EmitVertex();

    // p3
    local = p3_px - P0_px;
    pCurrent = vec2(dot(local, d1), dot(local, perp1));
    pProgress = 1.0; outColor = gs_in[1].color;
    gl_Position = vec4(((p3_px / uViewportSize - 0.5) * 2.0) * P1_clip.w, P1_clip.z, P1_clip.w);
    EmitVertex();

    EndPrimitive();
}
