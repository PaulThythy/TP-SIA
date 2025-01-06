#version 450

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragUV;

uniform vec3 cameraPosition;

uniform sampler2D textureSampler;
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
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(fragPosition - cameraPosition);
    vec3 R = refract(I, normalize(fragNormal), ratio);
    finalColor = vec4(texture(textureSampler, R).rgb, 1.0);

}
