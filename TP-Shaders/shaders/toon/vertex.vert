#version 450

uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

out vec3 fragPosition;
out vec3 fragNormal;

void main() {
    gl_Position = MVP * vec4(position, 1.0);

    vec4 posWorld = MODEL * vec4(position, 1.0);
    fragPosition = posWorld.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(MODEL)));
    fragNormal = normalize(normalMatrix * normal);
}
