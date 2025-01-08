#version 450

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragUV;

uniform vec3 cameraPosition;

uniform sampler2D textureSampler;
uniform samplerCube skybox;
uniform mat4 MODEL;

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
    vec3 I = normalize(fragPosition - cameraPosition);

    vec3 N = normalize(fragNormal);

    vec3 R = reflect(-I, N);

    vec3 envColor = texture(skybox, R).rgb;

    finalColor = vec4(envColor, 1.0);
}
