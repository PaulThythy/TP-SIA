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
    vec3 specular = pow(specAngle, materialShininess) * materialSpecularColor * light.intensities;

    // Attenuation si souhaitée
    float dist = length(light.position - fragPosition);
    float attenuation = 1.0 / (1.0 + light.attenuation * dist * dist);

    vec3 lightColor = (ambient + diffuse + specular) * attenuation;

    // On récupère la couleur de la texture
    vec4 texColor = texture(textureSampler, fragUV);

    // Couleur finale
    finalColor = vec4(lightColor * materialAlbedo, 1.0) * texColor;

}
