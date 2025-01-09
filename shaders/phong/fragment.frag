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
    // Normalisation de la normale
    vec3 N = normalize(fragNormal);

    // Vecteur direction de la lumière
    vec3 L = normalize(light.position - fragPosition);

    // Vecteur vue (direction de la caméra)
    vec3 V = normalize(cameraPosition - fragPosition);

    // Calcul de la composante ambiante
    vec3 ambient = light.ambientCoefficient * light.intensities;

    // Calcul de la composante diffuse (Lambert)
    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = diffuseFactor * light.intensities;

    // Calcul de la composante spéculaire
    vec3 R = reflect(-L, N);
    float specAngle = max(dot(R, V), 0.0);
    vec3 specular = pow(specAngle, material.shininess) * material.specularColor * light.intensities;

    // Attenuation (optionnelle)
    float dist = length(light.position - fragPosition);
    float attenuation = 1.0 / (1.0 + light.attenuation * dist * dist);

    vec3 lightColor = (ambient + diffuse + specular) * attenuation;

    // Couleur finale sans texture
    finalColor = vec4(lightColor * material.albedo, 1.0);

}
