#version 450

uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 position; // le location permet de dire de quel flux/canal on récupère les données (doit être en accord avec le location du code opengl)
layout(location = 2) in vec3 normal;

float outlineThickness = 0.01;

out vec3 fragPosition;
out vec3 fragNormal;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(MODEL)));
    vec3 N = normalize(normalMatrix * normal);

    vec3 inflatedPosition = position + N * outlineThickness;

    gl_Position = MVP * vec4(inflatedPosition, 1.0);
}


