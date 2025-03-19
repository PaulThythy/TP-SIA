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

vec3 coolColor = vec3(1.0, 0.6, 0.2);
vec3 warmColor = vec3(0.0, 0.3, 0.7);
float alpha = 0.7;    // Influence de la teinte chaude
float beta = 0.3;     // Influence de la teinte froide

void main() {
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(light.position - fragPosition);
    
    float NdL = dot(N, L) * 0.5 + 0.5; // Normalisation entre 0 et 1

    // Détermination des couleurs Gooch
    vec3 warm = mix(coolColor, warmColor, alpha);
    vec3 cool = mix(coolColor, warmColor, beta);

    vec3 goochColor = mix(cool, warm, NdL);

    vec3 V = normalize(cameraPosition - fragPosition);
    float outlineDotProduct = dot(V, fragNormal);

    if(outlineDotProduct <= 0.3) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        finalColor = vec4(goochColor * light.intensities, 1.0);
    }
}
