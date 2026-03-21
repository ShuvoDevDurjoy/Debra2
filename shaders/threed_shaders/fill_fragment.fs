#version 330 core
out vec4 FragColor;

in vec3 gFragPos;
in vec2 g_uv;
in vec3 gNormal;

uniform vec3 objectColor = vec3(0, 1.0, 1.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 lightPos = vec3(0.0, 0.0, 100.0);
uniform vec3 viewPos;

uniform float u_progress = 1.0;
uniform int u_generation_direction = 0;
uniform float u_fill_opacity = 1.0;

uniform int u_use_grid = 1;
uniform vec2 u_resolution = vec2(32.0, 32.0);

uniform float u_gloss = 0.5;
uniform float u_shadow = 0.2;

vec4 add_light(vec4 color,
               vec3 point,
               vec3 unit_normal,
               vec3 light_coords,
               float gloss,
               float shadow){
    if(gloss == 0.0 && shadow == 0.0) return color;

    // Flip normal if backfacing for two-sided lighting
    if (!gl_FrontFacing) {
        unit_normal *= -1.0;
    }

    // Camera is assumed to be in the positive Z direction in Manim's coordinate system
    // but here we use viewPos from the camera uniform.
    vec3 viewDir = normalize(viewPos - point);
    vec3 to_light = normalize(light_coords - point);
    
    // Specular (Reflection-based Gaussian)
    vec3 light_reflection = reflect(-to_light, unit_normal);
    float dot_prod = dot(normalize(light_reflection), viewDir);
    float shine = gloss * exp(-3.0 * pow(1.0 - max(dot_prod, 0.0), 2.0));
    
    // Diffuse / Darkening
    float dp2 = dot(to_light, unit_normal);
    float darkening = mix(1.0, max(dp2, 0.0), shadow);
    
    return vec4(
        darkening * mix(color.rgb, vec3(1.0), shine),
        color.a
    );
}

vec4 finalize_color(vec4 color,
                    vec3 point,
                    vec3 unit_normal,
                    vec3 light_coords,
                    float gloss,
                    float shadow){
    return add_light(color, point, unit_normal, light_coords, gloss, shadow);
}

void main()
{
    float val = (u_generation_direction == 0) ? g_uv.y : g_uv.x;
    if (val > u_progress) discard;
    
    float alpha = u_fill_opacity;
    vec3 baseColor = objectColor;
    float lineEffect = 0.0;

    if (u_use_grid != 0) {
        // Procedural crisp grid lines
        vec2 gridPos = g_uv * (u_resolution - 1.0);
        lineEffect = smoothstep(0.0, 0.03, 0.03 - abs(fract(gridPos.x + 0.5) - 0.5)) + 
                     smoothstep(0.0, 0.03, 0.03 - abs(fract(gridPos.y + 0.5) - 0.5));
        lineEffect = clamp(lineEffect, 0.0, 1.0);

        // Standard integer-based checkerboard for maximum stability
        ivec2 iPos = ivec2(floor(gridPos + 0.5));
        float check = float((iPos.x + iPos.y) % 2);
        
        vec3 color1 = objectColor;        // Cyan (or custom)
        vec3 color2 = vec3(1.0);          // White
        baseColor = mix(color1, color2, check);
    }

    vec4 color = vec4(baseColor, alpha);
    vec3 norm = normalize(gNormal);
    
    FragColor = finalize_color(
        color,
        gFragPos,
        norm,
        lightPos,
        u_gloss,
        u_shadow
    );

    // Mix in the procedural grid lines on top (slightly dimmer on the internal side)
    FragColor.rgb = mix(FragColor.rgb, vec3(1.0, 1.0, 1.0), lineEffect * (gl_FrontFacing ? 1.0 : 0.4));
}
