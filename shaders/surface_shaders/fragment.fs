// #version 330 core
// out vec4 FragColor;

// in vec3 FragPos;
// in vec3 Normal;

// uniform vec3 lightPos;
// uniform vec3 viewPos;
// uniform vec3 objectColor;
// uniform vec3 lightColor;

// in float pProgress;
// in float progress;

// void main()
// {
//     float edge = pProgress - progress;

//     // screen-space adaptive smoothing
//     float w = fwidth(pProgress);

//     float alpha1 = 1.0 - smoothstep(-w, w, edge);

//     if (alpha1 <= 0.01)
//         discard;
//     // ---- Ambient (slightly stronger, colored) ----
//     float ambientStrength = 0.15;
//     vec3 ambient = ambientStrength * lightColor;

//     // ---- Handle normals for two-sided surfaces ----
//     vec3 norm = normalize(Normal);
//     if (!gl_FrontFacing)
//         norm = -norm;

//     // ---- Diffuse lighting (Lambert) ----
//     vec3 lightDir = normalize(lightPos - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = diff * lightColor;

//     // ---- Specular (Phong) ----
//     float specularStrength = 0.2;
//     vec3 viewDir = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);
//     float shininess = 16.0; // softer, more natural highlights
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
//     vec3 specular = specularStrength * spec * lightColor;

//     // ---- Combine ----
//     vec3 lighting = ambient + diffuse + specular;

//     // ---- Apply object color in a more natural way ----
//     vec3 result = lighting * objectColor;

//     // Optional: add slight gamma correction for richer colors
//     result = pow(result, vec3(1.0/2.2));

//     FragColor = vec4(result, 1.0);
// }



#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in float v_triangleStart;
in float v_triangleEnd;
in float v_vertexInterp;
in vec3 obj_color;

flat in float triangleID;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float u_progress; // Make sure to pass u_progress here too!

void main()
{
    // 1. Calculate the exact progress point of THIS pixel
    // We map the vertex gradient (0-1) into the triangle's time slot
    float pixelProgress = v_triangleStart + (v_vertexInterp * (v_triangleEnd - v_triangleStart));

    // 2. Smooth Discard
    // Using fwidth creates a small anti-aliased edge instead of a jagged one
    float threshold = 0.001; // Adjust for sharpness
    // if (pixelProgress > u_progress)
    //     discard;

    float alpha = step(pixelProgress, u_progress);
//     float w = fwidth(pixelProgress);
// float alpha = smoothstep(u_progress + w, u_progress - w, pixelProgress);



    // ---- Lighting Logic (Same as your original) ----
    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing) norm = -norm;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
    vec3 lighting = (0.15 + diff + (0.2 * spec)) * lightColor;
    vec3 result = lighting * obj_color;

    // Gamma correction
    FragColor = vec4(pow(result, vec3(1.0/2.2)), alpha);
}