#version 450

in vec3 fragPosition;
in vec3 fragColor;

uniform vec3 cameraPosition;

uniform struct Material {
    float shininess;
    vec3 specularColor;
    vec3 albedo;
} material;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float ambientCoefficient;
	float attenuation;
} light;

out vec4 finalColor;

void main() {
    finalColor = vec4(fragColor, 1.0);
}
