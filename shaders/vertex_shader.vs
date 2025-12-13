// shaders/fullscreen.vs
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 mPos;
uniform float scale_factor;
uniform bool is_morphing;
uniform float morph_progress;
uniform float line_width;
uniform bool scale_with_zoom;
uniform vec2 position;
uniform vec2 translate;


void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0) * vec4(position, 0.0, 1.0) + vec4(translate, 0.0, 0.0);

    if(is_morphing){
        vec2 pos = mix(aPos, mPos, morph_progress);
        gl_Position = vec4(pos, 0.0, 1.0);
    }
}