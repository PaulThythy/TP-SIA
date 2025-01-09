#version 450

uniform sampler2D shadowMap;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float ambientCoefficient;
	float attenuation;
} light;

uniform vec3 cameraPosition;

out vec4 finalColor;

void main() {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + vec3(0.5, 0.5, 0.5);

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    float shadow = ShadowCalculation(fs_in.fragPosLightSpace);

    vec3 lighting = (light.ambientCoefficient + (1.0 - shadow) * (diffuse + specular)) * color;

    finalColor = vec4(lighting, 1.0);
}