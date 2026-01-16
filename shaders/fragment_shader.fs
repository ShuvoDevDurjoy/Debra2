// #version 330 core

// out vec4 outColor;
// void main(){
//     outColor = vec4(1.0, 0, 0, 1.0);
// }



// #version 330 core 
// out vec4 FragColor;
//  in vec3 FragPos;
//  in vec3 Normal;
//  in float v_triangleStart;
//  in float v_triangleEnd;
//  in float v_vertexInterp;
//  in vec3 obj_color;
//  flat in float triangleID;
//  uniform vec3 lightPos;
//  uniform vec3 viewPos;
//  uniform vec3 lightColor;
//  uniform float u_progress;
//  // Make sure to pass u_progress here too! 
//  flat in vec3 corner1, corner2, corner3;
//  float sdTriangle(vec2 p, vec2 a, vec2 b, vec2 c) { vec2 ba = b - a;
//  vec2 pa = p - a;
//  vec2 cb = c - b;
//  vec2 pb = p - b;
//  vec2 ac = a - c;
//  vec2 pc = p - c;
//  vec2 norBA = vec2(ba.y, -ba.x);
//  vec2 norCB = vec2(cb.y, -cb.x);
//  vec2 norAC = vec2(ac.y, -ac.x);
//  float s1 = sign(dot(norBA, pa));
//  float s2 = sign(dot(norCB, pb));
//  float s3 = sign(dot(norAC, pc));
//  if (s1 + s2 + s3 < 2.0) return min( min(length(pa - ba * clamp(dot(pa, ba)/dot(ba, ba), 0.0, 1.0)), length(pb - cb * clamp(dot(pb, cb)/dot(cb, cb), 0.0, 1.0))), length(pc - ac * clamp(dot(pc, ac)/dot(ac, ac), 0.0, 1.0)) );
//  return -min(min(dot(norBA, pa), dot(norCB, pb)), dot(norAC, pc));
//  } 
//   vec2 projectToTrianglePlane(vec3 p, vec3 a, vec3 b, vec3 c) { vec3 xAxis = normalize(b - a);
//  vec3 normal = normalize(cross(b - a, c - a));
//  vec3 yAxis = cross(normal, xAxis);
//  vec3 d = p - a;
//  return vec2(dot(d, xAxis), dot(d, yAxis));
//  } 


// void main() { 

// float pixelProgress = v_triangleStart + (v_vertexInterp * (v_triangleEnd - v_triangleStart));
//  float wProg = fwidth(pixelProgress);
//  float progressAlpha = smoothstep(u_progress + wProg, u_progress - wProg, pixelProgress);
//  if (progressAlpha <= 0.001) discard;

//  vec2 p = projectToTrianglePlane(FragPos, corner1, corner2, corner3);
//  vec2 a = projectToTrianglePlane(corner1, corner1, corner2, corner3);
//  vec2 b = projectToTrianglePlane(corner2, corner1, corner2, corner3);
//  vec2 c = projectToTrianglePlane(corner3, corner1, corner2, corner3);
//  float d = sdTriangle(p, a, b, c);
//  float w = fwidth(d);

//  //modification needed here
//  float sdfAlpha = 1.0 - smoothstep(0.0, w + 0.1, d);
//  if (sdfAlpha <= 0.1) discard;

// float alpha = sdfAlpha * progressAlpha;

//  // -------------------------------------------------- 
//  // 3. Lighting (UNCHANGED) 
//  // -------------------------------------------------- 
//  vec3 norm = normalize(Normal);
//  if (!gl_FrontFacing) norm = -norm;
//  vec3 lightDir = normalize(lightPos - FragPos);
//  float diff = max(dot(norm, lightDir), 0.0);
//  vec3 viewDir = normalize(viewPos - FragPos);
//  vec3 reflectDir = reflect(-lightDir, norm);
//  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
//  vec3 lighting = (0.15 + diff + (0.2 * spec)) * lightColor;
//  vec3 result = lighting * obj_color;
//  FragColor = vec4(1, 1, 0, alpha);
// //  FragColor = vec4(pow(result, vec3(1.0 / 2.2)), alpha);
//  }



// #version 330 core
// out vec4 FragColor;

// in vec3 FragPos;
// in vec3 Normal;
// in vec3 obj_color;

// in float v_triangleStart;
// in float v_triangleEnd;

// // Barycentric coordinates from GS
// in vec3 bary;

// flat in float triangleID;

// uniform vec3 lightPos;
// uniform vec3 viewPos;
// uniform vec3 lightColor;
// uniform float u_progress;

// flat in vec3 c;

// float sdfDistance(vec3 p, vec3 c1){
//     return length(p - c1);
// }

// void main()
// {
    
//     // ----------------------------
//     // 3. Lighting (Phong)
//     // ----------------------------
//     float d = length(FragPos - c);

//     float thickness = 10;   // adjust this
//     float aa = fwidth(d);
    
//     float alpha = 1.0 - smoothstep(
//         thickness - aa * 0.5,
//         thickness + aa * 0.5,
//         d
//     );
//     if(alpha <= 0.01) discard;
//     vec3 N = normalize(Normal);
//     if(!gl_FrontFacing) N = -N;

//     vec3 lightDir = normalize(lightPos - FragPos);
//     vec3 viewDir  = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, N);

//     float diff = max(dot(N, lightDir), 0.0);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

//     vec3 ambient  = 0.15 * lightColor;
//     vec3 diffuse  = diff * lightColor;
//     vec3 specular = 0.2 * spec * lightColor;

//     vec3 color = (ambient + diffuse + specular) * obj_color;

//     // Gamma correction
//     color = pow(color, vec3(1.0/2.2));
//     float width = fwidth(2);
//     // float alpha = smoothstep(-0.01, 0.01, d);

//     FragColor = vec4(1, 0, 0, 1);
// }
/*
*/

#version 330 core

out vec4 FragColor;

in vec3 v_FragPos; // Must be in World Space
in vec3 Normal;
flat in vec3 v_P0; // Must be in World Space
flat in vec3 v_P1;
flat in vec3 v_P2;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
in vec3 obj_color;

uniform float stroke_line_width = 0.05; 

float sdSegment(vec3 p, vec3 a, vec3 b) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

void main() {
    float dBase = sdSegment(v_FragPos, v_P0, v_P1);
    float dHeight = sdSegment(v_FragPos, v_P1, v_P2);
    float d = min(dBase, dHeight);

    // --- STABLE BLUR LOGIC ---
    
    // 1. Calculate the change in distance per pixel (screen-space)
    // This is the key to stopping rotation flicker.
    float unitPerPixel = fwidth(d);
    
    // 2. Define how many pixels wide the "blur" should be
    float blurWidth = unitPerPixel; 

    // 3. Calculate Alpha
    // We want alpha to be 1.0 inside the width, and 0.0 outside.
    float alpha = smoothstep(stroke_line_width * 0.05 + 0.01, stroke_line_width * 0.05 - 0.01, d);

    // 4. Discard logic
    if (alpha <= 0.01) discard;

    // --- LIGHTING ---
    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing) norm = -norm;
    
    vec3 lightDir = normalize(lightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 viewDir = normalize(viewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    
    vec3 lighting = (0.5 + diff + (0.3 * spec)) * lightColor;
    vec3 result = lighting * obj_color;

    // Gamma correction and apply alpha
    FragColor = vec4(pow(result, vec3(1.0 / 2.2)), alpha);
}