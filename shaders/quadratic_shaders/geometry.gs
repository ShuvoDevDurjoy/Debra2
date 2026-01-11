#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float uProgress;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 perpControl;
out vec3 prepStart;
out vec3 prevEnd;

out vec3 pCurrent;

uniform float u_line_width;

out float pProgress;
out float progress;

out float stroke_width;

uniform int vertexCount;  

in VS_OUT{
    vec3 cp;
    vec3 pP;
    vec3 nP;
    vec3 color;
} gs_in[];

flat in int vId[];

flat out int vertexId;
out vec3 outColor;
vec2 perpendicular(vec2 v)
{
    return vec2(-v.y, v.x);
}

vec2 norm(vec2 v){
    float len = length(v);
    if(len < 0.0001) return vec2(0, 0);
    return v / len;
}

float safeDot(vec2 v1, vec2 v2){
    float t = dot(v1, v2);
    return t < 0.001? 1: t;
}

void main()
{

    if(uProgress <= 0.001) return;
    float prevProgress = (vId[0] * 1.0) / (vertexCount - 1);
    float nextProgress = (vId[1] * 1.0) / (vertexCount - 1);

    // if(bool(abs(gs_in[0].cp.y) > 30) || bool(abs(gs_in[1].cp.y) > 30)) return;
    mat4 pvm = projection * view * model;
    if(uProgress <= prevProgress) return;
    // How much of THIS segment is visible
    float localProgress =
        clamp((uProgress - prevProgress) / (nextProgress - prevProgress), 0.0, 1.0);

    progress = localProgress;

    vec2 dir = gs_in[1].cp.xy - gs_in[0].cp.xy;

    vec2 perp = norm(vec2(-dir.y, dir.x));

    vec2 dir0 = norm(gs_in[0].cp.xy - gs_in[0].pP.xy);
    vec2 dir1 = norm(gs_in[1].cp.xy - gs_in[0].cp.xy);
    vec2 dir2 = norm(gs_in[1].nP.xy - gs_in[1].cp.xy);

    float dis = length(gs_in[1].cp.xyz - gs_in[0].cp.xyz);

    vec2 n0 = perpendicular(dir0);
    vec2 n1 = perpendicular(dir1);
    vec2 n2 = perpendicular(dir2);

    vec2 milter1 = norm(n0 + n1) * (1 / safeDot(norm(n0 + n1), n0));
    vec2 milter2 = norm(n1 + n2) * (1 / safeDot(norm(n1 + n2), n2));
    if(length(dir0) < 0.0001){
        milter1 = n1;
    }
    if(length(dir2) < 0.0001){
        milter2 = n1;
    }

    prepStart = gs_in[0].cp;
    prevEnd = gs_in[1].cp;

    vec2 midPoint = (gs_in[0].cp.xy + gs_in[1].cp.xy)  * 0.5;

    //modification starts from here
    float cosPrev = clamp(dot(dir0, dir1), -1.0, 1.0);
    float cosNext = clamp(dot(dir1, dir2), -1.0, 1.0);
    
    float anglePrev = acos(cosPrev);
    float angleNext = acos(cosNext);

    float signPrev = sign(dir1.x * dir0.y - dir1.y * dir0.x);
    float signNext = sign(dir2.x * dir1.y - dir2.y * dir1.x);

    float curvature = (anglePrev * signPrev + angleNext * signNext) * 0.1;

    float strength = u_line_width * 0.1;
    vec2 controlOffset = n1 * curvature - n1 * u_line_width * dis * 0.0005;

    vec2 avgNormal = norm(n0 + n2);
    
    perpControl = vec3(midPoint + controlOffset, 0.0);


    vec3 p0 = gs_in[0].cp - vec3(milter1.xy, 0) * u_line_width * 0.5;
    vec3 p1 = gs_in[0].cp + vec3(milter1.xy, 0) * u_line_width * 0.5;
    vec3 p2 = gs_in[1].cp - vec3(milter2.xy, 0) * u_line_width * 0.5;
    vec3 p3 = gs_in[1].cp + vec3(milter2.xy, 0) * u_line_width * 0.5;


    stroke_width = u_line_width * 0.5 * 0.25;


    vec2 base = norm(dir1);
    vec2 up = perpendicular(base);

    pCurrent = p0;
    pProgress = 0;
    // pCurrent = vec3(dot(norm(p0.xy), base), dot(norm(p0.xy), up), 0);
    gl_Position = pvm * vec4(p0, 1.0);
    outColor = gs_in[0].color;
    EmitVertex();

    pProgress = 0;
    pCurrent = p1;
    // pCurrent = vec3(dot(norm(p1.xy), base), dot(norm(p1.xy), up), 0);
    gl_Position = pvm * vec4(p1, 1.0);
    outColor = gs_in[0].color;
    EmitVertex();

    pProgress = 1;
    pCurrent = p2;
    // pCurrent = vec3(dot(norm(p2.xy), base), dot(norm(p2.xy), up), 0);
    gl_Position = pvm * vec4(p2, 1.0);
    outColor = gs_in[1].color;
    EmitVertex();

    pProgress = 1;
    pCurrent = p3;
    // pCurrent = vec3(dot(norm(p3.xy), base), dot(norm(p3.xy), up), 0);
    gl_Position = pvm * vec4(p3, 1.0);
    outColor = gs_in[1].color;
    EmitVertex();

    EndPrimitive();
}
