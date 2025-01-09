#version 450

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
    float bias = 0.0005;
    float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    float diffuse  = 0.7;  
    float specular = 0.3;   
    vec3 color     = material.albedo;

    float shadow = ShadowCalculation(fragPosLightSpace);

    float ambient = light.ambientCoefficient;

    vec3 lighting = (light.ambientCoefficient + (1.0 - shadow) * (diffuse + specular)) * color;

    finalColor = vec4(lighting, 1.0);
}