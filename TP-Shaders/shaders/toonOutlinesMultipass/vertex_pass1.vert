#version 450

uniform mat4 MVP;
uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;

float outlineThickness = 0.01;

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(MODEL)));
    vec3 N = normalize(normalMatrix * normal);

    vec3 inflatedPosition = position + N * outlineThickness;

    gl_Position = MVP * vec4(inflatedPosition, 1.0);
}