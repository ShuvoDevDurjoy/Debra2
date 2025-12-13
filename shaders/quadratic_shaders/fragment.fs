#version 330 core
out vec4 fragColor;

uniform float iTime;
uniform vec2  iResolution;
uniform vec3 color;


void main3(){
    fragColor = vec4(color, 1.0f);
}


void main(){
    main3();
}