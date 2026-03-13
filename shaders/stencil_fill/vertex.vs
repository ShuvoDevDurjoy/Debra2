#version 330 core

layout (location = 0) in vec3 currentPos;
layout (location = 1) in vec3 prevPoint;
layout (location = 2) in vec3 nextPoint;
layout (location = 3) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(currentPos, 1.0);
}
