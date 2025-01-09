#version 450

layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 MODEL;

void main() {
    gl_Position = lightSpaceMatrix * MODEL * vec4(position, 1.0);
}