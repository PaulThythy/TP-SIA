#version 450

in vec3 fragPosition;
in vec3 fragNormal;

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
    vec3 N = normalize(fragNormal);

    vec3 L = normalize(light.position - fragPosition);

    vec3 V = normalize(cameraPosition - fragPosition);

    vec3 ambient = light.ambientCoefficient * light.intensities;

    float diffuseFactor = max(dot(N, L), 0.0);

    float quantizedDiffuse;
    if (diffuseFactor < 0.3) {
        quantizedDiffuse = 0.0;
    } else if (diffuseFactor < 0.7) {
        quantizedDiffuse = 0.5;
    } else {
        quantizedDiffuse = 1.0;
    }

    vec3 diffuse = quantizedDiffuse * light.intensities;

    vec3 specular = vec3(0.0);

    float dist = length(light.position - fragPosition);
    float attenuation = 1.0 / (1.0 + light.attenuation * dist * dist);

    vec3 lightColor = (ambient + diffuse + specular) * attenuation;

    float outlineDotProduct = dot(V, fragNormal);

    if(outlineDotProduct <= 0.3) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        finalColor = vec4(lightColor * material.albedo, 1.0);
    }
}
