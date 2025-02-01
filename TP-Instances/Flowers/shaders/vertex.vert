#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

layout(location = 2) in vec3 instanceOffset; // Position de l'instance
layout(location = 3) in float instanceScale; // Échelle de l'instance
layout(location = 4) in float instanceRotation; // Rotation de l'instance (en radians)

uniform mat4 MVP; // Matrice Model-View-Projection
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

out vec2 UV;

void main() {
    // Matrice d'identité pour commencer
    mat4 model = mat4(1.0);

    // Appliquer la translation
    model[3] = vec4(instanceOffset, 1.0);

    // Appliquer la rotation autour de l'axe Y
    float cosTheta = cos(instanceRotation);
    float sinTheta = sin(instanceRotation);
    mat4 rotation = mat4(
        vec4(cosTheta, 0.0, -sinTheta, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(sinTheta, 0.0, cosTheta, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    model *= rotation;

    // Appliquer l'échelle
    mat4 scale = mat4(
        vec4(instanceScale, 0.0, 0.0, 0.0),
        vec4(0.0, instanceScale, 0.0, 0.0),
        vec4(0.0, 0.0, instanceScale, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    model *= scale;

    // Calculer la position finale des sommets
    gl_Position = MVP * model * vec4(vertexPosition, 1.0);

    // Passer les coordonnées UV au fragment shader
    UV = vertexUV;
}
