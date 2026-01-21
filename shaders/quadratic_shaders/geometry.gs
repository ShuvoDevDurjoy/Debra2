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


// vec3 pickOrthogonal(vec3 v)
// {
//     // v must be normalized
//     if (abs(v.x) < abs(v.y))
//     {
//         if (abs(v.x) < abs(v.z))
//             return vec3(0, 0, 1);
//         else
//             return vec3(1, 0, 0);
//     }
//     else
//     {
//         if (abs(v.y) < abs(v.z))
//             return vec3(0, 0, 1);
//         else
//             return vec3(0, 1, 0);
//     }
// }

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



void main()
{
    if(uProgress <= 0.001) return;


    float prevProg = float(vId[0]) / float(vertexCount - 1);
    float nextProg = float(vId[1]) / float(vertexCount - 1);
    if(uProgress <= prevProg) return;
    progress = clamp((uProgress - prevProg)/(nextProg - prevProg), 0.0, 1.0);
    float selfBias = float(vId[0]) * 1e-6;
    float z_bias = float(u_layer + vId[0]) * 1e-6;
    // z_bias = z_bias - selfBias;


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
    if(length(planeN) < 1e-4){
        vec3 ref = vec3(0, 0, -1);
        if(dot((P1 - P0), vec3(0, 1, 0)) < 0.001){
            ref = vec3(-1, 0, 0);
        }
        else if(dot((P1 - P0), vec3(0, -1, 0)) < 0.001){
            ref = vec3(-1, 0, 0);
        }
        else if(dot((P1 - P0), vec3(1, 0, 0)) < 0.001){
            ref = vec3(1, 1, 0);
        }
        else if(dot((P1 - P0), vec3(-1, 0, 0)) < 0.001){
            ref = vec3(0, -1, 0);
        }

        ref = pickOrthogonal(d1);
        planeN = cross(d1, ref);
        // planeN = norm3(cross(d1, ref));
    }

    vec3 perp0 = norm3(cross(planeN, d0));
    vec3 perp1 = norm3(cross(planeN, d1));
    vec3 perp2 = norm3(cross(planeN, d2));

    vec3 miterStart = isStart ? norm3(perp1 + perp0) : norm3(perp0 + perp1);
    vec3 miterEnd   = isEnd   ? norm3(perp1 + perp2) : norm3(perp1 + perp2);

    float segLen = length(P1-P0);
    // if(segLen < 0.0001) return;
    float miterLimit = max(u_line_width, segLen * 0.5);

    float miterScaleStart = isStart ? 1.0 : 1.0 / max(dot(miterStart, perp1), 0.25);
    float miterScaleEnd   = isEnd   ? 1.0 : 1.0 / max(dot(miterEnd, perp1), 0.25);

    float halfW = u_line_width * 0.5;
    float lengthStart = halfW * miterScaleStart;
    float lengthEnd = halfW * miterScaleEnd;

    float ratio = lengthStart / lengthEnd;

    lengthStart = min(lengthStart, miterLimit);
    lengthEnd = min(lengthEnd, miterLimit);

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
    bool useBezier = abs(theta) < CURVE_THRESHOLD;

    float tPrev = tan(anglePrev * 0.5);
    float tNext = tan(angleNext * 0.5);
    
    float totalBend = abs(tPrev) + abs(tNext);
    float bias = 0.5; // default center
    
    bias = (0.5 + 0.5 * (tNext - tPrev) / totalBend); 
    float controlX = segLen * 0.5;
    //user_bezier_always == 1.0 && (abs(theta) <=radians(45.0))) ||
    if((useBezier && !isStart && !isEnd)){
        controlY = (isStart ? 0.01 + tan(theta * 0.5): 0.01 + tan(theta * 0.5) * segLen * 0.5);
        controlX = segLen * bias;
    }

    perpControl = vec2(controlX, controlY);

    prepStart = vec2(0.0,0.0);
    prevEnd = vec2(segLen,0.0);
    stroke_width = halfW * 0.5;

    // --- Expand quad to cover curve at corners ---
    float safeControlY = clamp(abs(controlY), 0.0, segLen * 0.5);
    // float rate = safeControlY / controlY;
    float expand = 0.5 + (safeControlY / halfW);
    expand = max(0.5, expand);
    // float expand = 0.5 +  abs(controlY)/halfW;
    float curvature = abs(tPrev) + abs(tNext);

    vec3 p0 = P0 - offsetStart * expand ;
    vec3 p1 = P0 + offsetStart * expand;
    vec3 p2 = P1 - offsetEnd * expand;
    vec3 p3 = P1 + offsetEnd * expand;

    // --- Local 2D basis ---
    vec3 T = d1;
    vec3 N = perp1;
    vec3 origin = P0;

    // --- Emit vertices ---
    vec3 local = p0-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 0.0; outColor = gs_in[0].color;
    vec4 clip = pvm*vec4(p0, 1.0); 
    clip.z -= z_bias * clip.w;
    gl_Position = clip;
    EmitVertex();

    local = p1-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 0.0; outColor = gs_in[0].color;
    clip = pvm*vec4(p1,1.0); 
    clip.z -= z_bias * clip.w;
    gl_Position = clip;
    EmitVertex();

    local = p2-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 1.0; outColor = gs_in[1].color;
    clip = pvm*vec4(p2, 1.0);
    clip.z -= z_bias * clip.w;
    gl_Position = clip; 
    EmitVertex();

    local = p3-origin;
    pCurrent = vec2(dot(local,T), dot(local,N));
    pProgress = 1.0; outColor = gs_in[1].color;
    clip = pvm*vec4(p3, 1.0); 
    clip.z -= z_bias * clip.w;
    gl_Position = clip;
    EmitVertex();

    EndPrimitive();
}
