#version 450

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

out vec3 fragPosition;
out vec3 fragNormal;
out vec4 fragPosLightSpace;

uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;
uniform mat4 lightSpaceMatrix;

void main() {
    fragPosition = vec3(MODEL * vec4(position, 1.0));
    fragNormal = transpose(inverse(mat3(MODEL))) * normal;
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPosition, 1.0);
    gl_Position = PROJECTION * VIEW * vec4(fragPosition, 1.0);
}