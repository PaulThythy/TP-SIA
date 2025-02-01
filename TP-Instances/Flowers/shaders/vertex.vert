#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

layout(location = 2) in vec3 instanceOffset; // Position de l'instance
layout(location = 3) in float instanceScale; // Échelle de l'instance
layout(location = 4) in float instanceRotation; // Rotation de l'instance (en radians)
layout(location = 5) in int instanceTextureType; // Type de texture pour l'instance
layout(location = 6) in int instanceHasFlower; // Indique si l'instance a une fleur

uniform mat4 MVP; // Matrice Model-View-Projection

out vec2 UV;
flat out int TextureType;
flat out int HasFlower;

const float PI = 3.14159265359;

void main() {
    mat4 model = mat4(1.0);
    model[3] = vec4(instanceOffset, 1.0);

    float cosTheta = cos(instanceRotation);
    float sinTheta = sin(instanceRotation);
    mat4 rotation = mat4(
        vec4(cosTheta, 0.0, -sinTheta, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(sinTheta, 0.0, cosTheta, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    model *= rotation;

    mat4 scale = mat4(
        vec4(instanceScale, 0.0, 0.0, 0.0),
        vec4(0.0, instanceScale, 0.0, 0.0),
        vec4(0.0, 0.0, instanceScale, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    model *= scale;

    int faceIndex = gl_InstanceID % 3; 
    float angle = faceIndex * (PI / 3.0);
    float cosFace = cos(angle);
    float sinFace = sin(angle);
    mat4 faceRotation = mat4(
        vec4(cosFace, 0.0, -sinFace, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(sinFace, 0.0, cosFace, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    model *= faceRotation;

    gl_Position = MVP * model * vec4(vertexPosition, 1.0);

    UV = vertexUV;
    TextureType = instanceTextureType;
    HasFlower = instanceHasFlower;
}