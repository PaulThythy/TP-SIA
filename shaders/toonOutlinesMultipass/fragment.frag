#version 450

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 cameraPosition;

uniform float materialShininess;
uniform vec3 materialSpecularColor;
uniform vec3 materialAlbedo;

uniform struct Light {
    vec3 position;
    vec3 intensities;
    float ambientCoefficient;
    float attenuation;
} light;

out vec4 finalColor;

void main() {
    finalColor = vec4(1.0, 0.0, 0.0, 1.0);
}
