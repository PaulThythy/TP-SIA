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

void main() 
{
    // Normalisation de la normale
    vec3 N = normalize(fragNormal);

    // Vecteur direction de la lumière
    vec3 L = normalize(light.position - fragPosition);

    // Vecteur vue (direction de la caméra)
    vec3 V = normalize(cameraPosition - fragPosition);

    //--------------------------------
    // Composante ambiante
    //--------------------------------
    vec3 ambient = light.ambientCoefficient * light.intensities;

    //--------------------------------
    // Composante diffuse “toon”
    //--------------------------------
    // On calcule d’abord le dot product classique
    float lambert = max(dot(N, L), 0.0);

    // Puis on le quantifie en paliers
    // Par exemple, 3 niveaux : 0.0, 0.5, 1.0
    // (vous pouvez ajuster le nombre de paliers)
    float toonLevels = 3.0;          // 3 paliers
    float toonValue  = floor(lambert * toonLevels) / (toonLevels - 1.0);

    // Composante diffuse
    vec3 diffuse = toonValue * light.intensities;

    //--------------------------------
    // Composante spéculaire (option toon)
    //--------------------------------
    // On peut aussi appliquer un seuil sur la spéculaire
    vec3 R = reflect(-L, N);
    float specAngle = max(dot(R, V), 0.0);

    float specSteps = 2.0; // par exemple, 2 niveaux (all or nothing)
    float specValue = floor(pow(specAngle, material.shininess) * specSteps);

    vec3 specular = specValue * material.specularColor * light.intensities;

    //--------------------------------
    // Attenuation (optionnelle)
    //--------------------------------
    float dist = length(light.position - fragPosition);
    float attenuation = 1.0 / (1.0 + light.attenuation * dist * dist);

    //--------------------------------
    // Couleur finale
    //--------------------------------
    vec3 lightColor = (ambient + diffuse + specular) * attenuation;
    finalColor = vec4(lightColor * material.albedo, 1.0);
}
