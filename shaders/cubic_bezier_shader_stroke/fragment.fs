// #version 330 core

// out vec4 FragColor;

// void main(){
//     FragColor = vec4(1.0, 0, 0, 1);
// }


// #version 330 core
// out vec4 FragColor;
// in vec2 vUV;

// void main(){
//     // Standard Quadratic Bezier SDF math: 
//     // The curve is defined by the boundary where u^2 - v = 0
//     float sdf = vUV.x * vUV.x - vUV.y;
    
//     // Calculate the derivative for anti-aliasing (smooth edges)
//     float delta = fwidth(sdf);
//     float opacity = smoothstep(-delta, delta, sdf);

//     // To make it a thin line instead of a filled shape:
//     float thickness = 0.05;
//     float line = smoothstep(thickness + delta, thickness, abs(sdf));

//     if(line < 0.1) discard; // Optimization: don't draw transparent pixels

//     FragColor = vec4(1.0, 0.0, 0.0, line); 
// }
#version 330 core

in vec2 uv;
in vec2 b2;
in float height;
in float stroke_w;
in float aa_w;

out vec4 frag;

float sdSegment(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

float dot2( in vec2 v ) { return dot(v,v); }


// Quadratic Bezier
vec2 bezier(vec2 A, vec2 B, vec2 C, float t)
{
    float u = 1.0 - t;
    return u*u*A + 2.0*u*t*B + t*t*C;
}

vec2 bezier_derivative(vec2 A, vec2 B, vec2 C, float t)
{
    return 2.0 * ((1.0 - t) * (B - A) + t * (C - B));
}

void main()
{
    // ----- Normalize space -----
    vec2 p = uv;
    p.y /= height;

    // ----- Bezier control points -----
    vec2 P0 = vec2(0.0, 0.0);
    vec2 P1 = b2;
    vec2 P2 = vec2(1.0, 0.0);

    // ----- Closest point via Newton iteration -----
    float t = clamp(p.x, 0.0, 1.0);

    for (int i = 0; i < 5; i++)
    {
        vec2 q  = bezier(P0, P1, P2, t);
        vec2 dq = bezier_derivative(P0, P1, P2, t);

        float denom = dot(dq, dq);
        if (denom < 1e-5) break;

        t -= dot(q - p, dq) / denom;
        t = clamp(t, 0.0, 1.0);
    }

    vec2 closest = bezier(P0, P1, P2, t);
    float d = length(p - closest);

    // ----- Normalize stroke width -----
    float hw = (stroke_w / height) * 0.5;
    float aa = aa_w / height;

    float alpha = smoothstep(hw + aa *0.5, hw, d);
    alpha = clamp(0, 1, alpha);
    if (alpha <= 0.0) discard;

    frag = vec4(1.0, 0.0, 0.0, alpha * 0.9);
}

