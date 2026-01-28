#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float uProgress;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float u_line_width;
uniform int vertexCount;
uniform int u_layer;
uniform float uMiterLimit = 2.0;
uniform int uStrokeJoinStyle = 0;  // 0=MITER, 1=BEVEL, 2=ROUND

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

const float CURVE_THRESHOLD = radians(12.0); // good default


vec3 pickOrthogonal(vec3 v)
{
    if(abs(v.x) < 0.0001 && abs(v.y) < 0.0001){
        return vec3(1, 0, 0);
    }
    else if(abs(v.x) < 0.0001 && abs(v.z) < 0.0001){
        return vec3(1, 0, 0);
    }
    else{
        return vec3(0, 1, 0);
    }
}
bool intersectSegments3D(
    vec3 P0, vec3 P1,
    vec3 Q0, vec3 Q1,
    out vec3 I
) {
    vec3 U = P1 - P0;
    vec3 V = Q1 - Q0;
    vec3 W0 = P0 - Q0;

    vec3 N = cross(U, V);
    float denom = dot(N, N);

    // Parallel
    if (denom < 1e-6) return false;

    // Not coplanar
    if (abs(dot(W0, N)) > 1e-6) return false;

    float t = dot(cross(W0, V), N) / denom;
    float s = dot(cross(W0, U), N) / denom;

    if (t < 0.0 || t > 1.0 || s < 0.0 || s > 1.0)
        return false;

    I = P0 + t * U;
    return true;
}



void main()
{
    if(uProgress <= 0.01) return;

    float prevProg = float(vId[0]) / float(vertexCount - 1);
    float nextProg = float(vId[1]) / float(vertexCount - 1);
    if(uProgress <= prevProg) return;
    progress = clamp((uProgress - prevProg)/(nextProg - prevProg), 0.0, 1.0);
    float selfBias = float(vId[0]) * 1e-6;
    float z_bias = float(u_layer + vId[0]) * 1e-6;

    mat4 pvm = projection * view * model;

    vec3 P0 = gs_in[0].cp;
    vec3 P1 = gs_in[1].cp;
    vec3 PP = gs_in[0].pP;
    vec3 NP = gs_in[1].nP;

    bool isStart = length(P0 - PP) < 1e-5;
    bool isEnd   = length(P1 - NP) < 1e-5;

    // --- segment directions ---
    vec3 d1 = norm3(P1 - P0);
    vec3 d0 = isStart ? d1 : norm3(P0 - PP);
    vec3 d2 = isEnd   ? d1 : norm3(NP - P1);

    // --- stable perpendicular ---
    vec3 planeN = norm3(cross(d1, d0));

    // Handle degenerate cases with better fallback
    if(length(planeN) < 1e-3){
        vec3 ref = pickOrthogonal(d1);
        planeN = norm3(cross(d1, ref));
        
        // If still degenerate, use safe default
        if(length(planeN) < 1e-3) {
            planeN = vec3(0, 0, 1);
        }
    }

    vec3 perp0 = norm3(cross(planeN, d0));
    vec3 perp1 = norm3(cross(planeN, d1));
    vec3 perp2 = norm3(cross(planeN, d2));
    
    // Ensure perpendiculars are valid
    if(length(perp0) < 1e-3) perp0 = perp1;
    if(length(perp2) < 1e-3) perp2 = perp1;

    vec3 miterStart = isStart ? norm3(perp1 + perp0) : norm3(perp0 + perp1);
    vec3 miterEnd   = isEnd   ? norm3(perp1 + perp2) : norm3(perp1 + perp2);
    
    // Fallback for degenerate miters
    if(length(miterStart) < 1e-3) miterStart = perp1;
    if(length(miterEnd) < 1e-3) miterEnd = perp1;

    float segLen = length(P1-P0);
    
    // Use shader uniform for miter limit (industry-standard: 2.0-4.0)
    float MITER_LIMIT = uMiterLimit; 
    
    // Calculate proper miter scale with clamping
    float miterScaleStart = isStart ? 1.0 : 1.0 / max(abs(dot(miterStart, perp1)), 0.25);
    float miterScaleEnd   = isEnd   ? 1.0 : 1.0 / max(abs(dot(miterEnd, perp1)), 0.25);
    
    // Extra safety clamp
    miterScaleStart = max(0.5, min(miterScaleStart, MITER_LIMIT));
    miterScaleEnd = max(0.5, min(miterScaleEnd, MITER_LIMIT));

    float halfW = u_line_width * 0.5;
    float lengthStart = halfW * miterScaleStart;
    float lengthEnd = halfW * miterScaleEnd;

    float ratio = lengthStart / lengthEnd;

    float safetyCap = 0.5;

    // lengthStart = min(lengthStart, miterLimit);
    // lengthEnd = min(lengthEnd, miterLimit);

    float epsilon = 0.1;
    vec3 offsetStart = miterStart * lengthStart ;
    vec3 offsetEnd   = miterEnd   * lengthEnd;

    
    float rad = radians(30.0);

    // --- Compute Bezier control based on corner curvature ---
    float anglePrev = acos(clamp(dot(d0,d1), 0, 1.0));
    float angleNext = acos(clamp(dot(d1,d2), 0, 1.0));
    float signPrev = sign(dot(cross(d1,d0), planeN));
    float signNext = sign(dot(cross(d2,d1), planeN));

    anglePrev *= signPrev;
    angleNext *= signNext;

    // Average corner effect
    float theta = 0.5 * (anglePrev + angleNext);
    // float maxTheta = radians(120.0);
    // theta = clamp(theta, -maxTheta, maxTheta);

    // if(segLen <= stroke_width * 2) return;
    float controlY = 0.01;
    // float controlY = 0.01;
    
    // Industry-standard sharp corner detection
    // Use stricter threshold for sharper corners
    const float SHARP_CORNER_THRESHOLD = radians(25.0); // 25 degree threshold
    bool useBezier = abs(theta) < SHARP_CORNER_THRESHOLD;

    float tPrev = tan(anglePrev * 0.5);
    float tNext = tan(angleNext * 0.5);
    
    float totalBend = abs(tPrev) + abs(tNext);
    float bias = 0.5; // default center
    
    // Improved bias calculation that handles sharp corners better
    if(totalBend > 0.01) {
        bias = clamp(0.5 + 0.5 * (tNext - tPrev) / totalBend, 0.1, 0.9);
    }
    float controlX = segLen * 0.5;
    
    // For sharp corners, use minimal bezier curve or straight line segment
    if(user_bezier_always == 1.0) {
        if(abs(theta) <= radians(60.0)) {
            controlY = tan(theta * 0.5) * segLen * 0.4;
            controlX = segLen * bias;
        } else {
            // Very sharp corner: minimize curve
            controlY = sign(theta) * 0.005 * u_line_width;
            controlX = segLen * 0.5;
        }
    } else if(useBezier && !isStart && !isEnd) {
        controlY = tan(theta * 0.5) * segLen * 0.5;
        controlX = segLen * bias;
    } else if(abs(theta) > radians(45.0) && !isStart && !isEnd) {
        // Ultra-sharp corner: use bevel instead of curve
        controlY = 0.0;
        controlX = segLen * 0.5;
    }

    perpControl = vec2(controlX, controlY);

    prepStart = vec2(0.0,0.0);
    prevEnd = vec2(segLen,0.0);
    stroke_width = halfW;

    // --- Expand quad to cover curve at corners (industry-standard approach) ---
    // The expansion must be sufficient to cover the bezier curve
    float safeControlY = clamp(abs(controlY), 0.0, halfW * 1.5);
    
    // Calculate expansion based on bezier control point offset
    // Smoother, more conservative expansion to avoid artifacts
    float expand = 1.0;
    if(abs(controlY) > 0.05) {
        // Only expand for meaningful curves
        expand = 1.0 + clamp(abs(controlY) / (halfW * 2.0), 0.0, 0.6);
    }
    
    // For very tight curves, minimize expansion to reduce artifacts
    if(abs(theta) > radians(40.0)) {
        expand = min(expand, 1.2);  // Limit expansion at sharp corners
    }
    
    // Overall cap to prevent excessive vertex displacement
    expand = min(expand, 1.5);

    vec3 p0 = P0 - offsetStart * expand ;
    vec3 p1 = P0 + offsetStart * expand;
    vec3 p2 = P1 - offsetEnd * expand;
    vec3 p3 = P1 + offsetEnd * expand;

    vec3 I;
    if(intersectSegments3D(P0, p0, P1, p2, I)){
        vec3 temp = p0;
        p0 = p2;
        p2 = temp;
    }
    if(intersectSegments3D(P0, p1, P1, p3, I)){
        vec3 temp = p1;
        p1 = p3;
        p3 = temp;
    }

    // vec3 center_min_top = 

    // --- Local 2D basis ---
    vec3 T = d1;
    vec3 N = perp1;
    vec3 origin = P0;

    // --- Emit vertices ---
    vec3 local = p0-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 0.0; outColor = gs_in[0].color;
    vec4 clip = pvm*vec4(p0, 1.0); 
    // clip.z -= z_bias * clip.w;
    gl_Position = clip;
    EmitVertex();

    local = p1-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 0.0; outColor = gs_in[0].color;
    clip = pvm*vec4(p1,1.0); 
    // clip.z -= z_bias * clip.w;
    gl_Position = clip;
    EmitVertex();

    local = p2-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 1.0; outColor = gs_in[1].color;
    clip = pvm*vec4(p2, 1.0);
    // clip.z -= z_bias * clip.w;
    gl_Position = clip; 
    EmitVertex();

    local = p3-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 1.0; outColor = gs_in[1].color;
    clip = pvm*vec4(p3, 1.0); 
    // clip.z -= z_bias * clip.w;
    gl_Position = clip;
    EmitVertex();

    EndPrimitive();
}
