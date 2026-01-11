// #version 330 core

// out vec4 FragColor;

// uniform vec3 fillColor;

// void main(){
//     FragColor = vec4(1, 0, 0, 1);
// }


#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float fillOpacity;

float dot2( in vec2 v ) { return dot(v,v); }

in float pProgress;
in float triProgress;

void main()
{

    if(pProgress > triProgress) discard;
    vec3 color = lightColor;
    // ---- Ambient ----
    float ambientStrength = 0.1;
    
    vec3 ambient = ambientStrength * color;

    // ---- Normal handling (two-sided surfaces) ----
    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing)
        norm = -norm;

    // ---- Diffuse ----
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // ---- Specular (Phong) ----
    float specularStrength = 0.6;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * color;

    // ---- Final color ----
    vec3 result = (ambient + diffuse + specular) * (objectColor);
    FragColor = vec4(objectColor,fillOpacity);
}
