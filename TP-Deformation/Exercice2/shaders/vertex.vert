#version 450

uniform mat4 MVP;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 color;

out vec3 fragPosition;
out vec3 fragColor;

void main() {
    gl_Position = MVP * vec4(position, 1.0);

    vec4 posWorld = MODEL * vec4(position, 1.0);
    fragPosition = posWorld.xyz;

    fragColor = color;
}