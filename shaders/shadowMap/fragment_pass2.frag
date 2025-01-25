#version 450

in vec3 fragPosition;
in vec3 fragNormal;
in vec4 fragPosLightSpace;

uniform sampler2D shadowMap;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float ambientCoefficient;
	float attenuation;
} light;

uniform struct Material {
    float shininess;
    vec3 specularColor;
    vec3 albedo;
} material;

uniform vec3 cameraPosition;

out vec4 finalColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
    // if outside the light frustum, no shadow
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(light.position - fragPosition);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.intensities;

    vec3 ambient = light.ambientCoefficient * light.intensities;

    float shadow = ShadowCalculation(fragPosLightSpace);

    vec3 result = (ambient + (1.0 - shadow) * (diffuse)) * material.albedo;

    finalColor = vec4(result, 1.0);
}