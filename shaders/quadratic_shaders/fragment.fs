// #version 330 core

// out vec4 FragColor;

// uniform vec3 objectColor;

// in float pProgress;
// in float progress;
// in vec3 outColor;

// void main()
// {
//     float edge = pProgress - progress;

//     // screen-space adaptive smoothing
//     float w = fwidth(pProgress);

//     float alpha = 1.0 - smoothstep(-w, w, edge);

//     if (alpha <= 0.001)
//         discard;

//     FragColor = vec4(outColor, alpha);
// }


#version 330 core

out vec4 FragColor;

uniform vec3 objectColor;

in float pProgress;
in float progress;
in vec3 outColor;

in float stroke_width;


in vec2 perpControl;
in vec2 prepStart;
in vec2 prevEnd;

in vec2 pCurrent;


float dot2( in vec2 v ) { return dot(v,v); }


float sdBezier( in vec2 pos, in vec2 A, in vec2 B, in vec2 C )
{    
    vec2 a = B - A;
    vec2 b = A - 2.0*B + C;
    vec2 c = a * 2.0;
    vec2 d = A - pos;
    float kk = 1.0/dot(b,b);
    float kx = kk * dot(a,b);
    float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;
    float kz = kk * dot(d,a);      
    float res = 0.0;
    float p = ky - kx*kx;
    float p3 = p*p*p;
    float q = kx*(2.0*kx*kx-3.0*ky) + kz;
    float h = q*q + 4.0*p3;
    if( h >= 0.0) 
    { 
        h = sqrt(h);
        vec2 x = (vec2(h,-h)-q)/2.0;
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        float t = clamp( uv.x+uv.y-kx, 0.0, 1.0 );
        res = dot2(d + (c + b*t)*t);
    }
    else
    {
        float z = sqrt(-p);
        float v = acos( q/(p*z*2.0) ) / 3.0;
        float m = cos(v);
        float n = sin(v)*1.732050808;
        vec3  t = clamp(vec3(m+m,-n-m,n-m)*z-kx,0.0,1.0);
        res = min( dot2(d+(c+b*t.x)*t.x),
                   dot2(d+(c+b*t.y)*t.y) );
        // the third root cannot be the closest
        res = min(res,dot2(d+(c+b*t.z)*t.z));
    }
    return sqrt( res );
}

void runMain(){
    float edge = pProgress - progress;

    // screen-space adaptive smoothing
    float w = fwidth(pProgress);

    float alpha1 = 1.0 - smoothstep(-w, w, edge);

    if (alpha1 <= 0.01)
        discard;



    float d = sdBezier(pCurrent.xy,
                   prepStart,
                   perpControl,
                   prevEnd);

float radius = stroke_width * 0.25;
float alpha = smoothstep(radius + radius * 0.5,
                         radius - radius * 0.5,
                         d);
if(alpha <= 0.01) discard;

FragColor = vec4(outColor, alpha);


}

void debugMain(){

    float edge = pProgress - progress;

    // screen-space adaptive smoothing
    float w = fwidth(pProgress);

    float alpha1 = 1.0 - smoothstep(-w, w, edge);

    if (alpha1 <= 0.01)
        discard;



//     float d = sdBezier(pCurrent.xy,
//                    prepStart,
//                    perpControl,
//                    prevEnd);

// float radius = stroke_width * 0.25;
// float alpha = smoothstep(radius + radius * 0.5,
//                          radius - radius * 0.5,
//                          d);
// if(alpha <= 0.01) discard;

FragColor = vec4(outColor, 1);

}

void debugMainWithBezier(){

    float edge = pProgress - progress;

    // screen-space adaptive smoothing
    float w = fwidth(pProgress);

    float alpha1 = 1.0 - smoothstep(-w, w, edge);

    // if (alpha1 <= 0.0001)
    //     discard;



    float d = sdBezier(pCurrent,
                   prepStart,
                   perpControl,
                   prevEnd);

float radius = stroke_width * 0.5;
float alpha = smoothstep(radius + radius * 0.5,
                         radius - radius * 0.5,
                         d);
if(alpha <= 0.0001) discard;

FragColor = vec4(outColor, 1);

}

void main()
{

    runMain();
    // debugMain();
    // debugMainWithBezier();

}
