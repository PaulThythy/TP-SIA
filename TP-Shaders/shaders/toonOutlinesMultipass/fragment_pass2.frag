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

    // Vecteur vue (direction caméra)
    vec3 V = normalize(cameraPosition - fragPosition);

    // Calcul de l'ambiante
    vec3 ambient = light.ambientCoefficient * light.intensities;

    // Calcul de la composante diffuse (Lambert)
    float diffuseFactor = max(dot(N, L), 0.0);

    // Quantification du diffuseFactor 
    // Par exemple sur 3 niveaux : 0.0, 0.5, 1.0
    float quantizedDiffuse;
    if (diffuseFactor < 0.3) {
        quantizedDiffuse = 0.0;
    } else if (diffuseFactor < 0.7) {
        quantizedDiffuse = 0.5;
    } else {
        quantizedDiffuse = 1.0;
    }

    vec3 diffuse = quantizedDiffuse * light.intensities;

    // Pour un toon shader très « cartoon », on peut ignorer le spéculaire 
    // ou le déclencher seulement sur un angle très précis.
    // Ici on l'ignore pour simplifier. On pourrait faire :
    // vec3 R = reflect(-L, N);
    // float specAngle = max(dot(R, V), 0.0);
    // vec3 specular = (specAngle > 0.95) ? materialSpecularColor * light.intensities : vec3(0.0);
    // Mais on reste simple et met specular à 0
    vec3 specular = vec3(0.0);

    // Calcul de l'atténuation (optionnel, souvent ignoré en toon shading)
    float dist = length(light.position - fragPosition);
    float attenuation = 1.0 / (1.0 + light.attenuation * dist * dist);

    vec3 lightColor = (ambient + diffuse + specular) * attenuation;

    // Couleur finale, pas de texture, juste l'albedo
    finalColor = vec4(lightColor * material.albedo, 1.0);
}
