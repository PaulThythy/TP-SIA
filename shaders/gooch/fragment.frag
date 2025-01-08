#version 450

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 cameraPosition;

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

vec3 coolColor = vec3(0.0, 0.0, 1.0);
vec3 warmColor = vec3(1.0, 0.6, 0.0);

void main() {
    // Normale normalisée
    vec3 N = normalize(fragNormal);
    // Vecteur direction lumière
    vec3 L = normalize(light.position - fragPosition);

    // Terme “Lambert” = N·L (borné à [0,1])
    float ndotL = max(dot(N, L), 0.0);

    // 1) Calcul de la “base Gooch”
    // Gooch mélange entre coolColor et warmColor selon ndotL
    // alpha = (1 + ndotL) / 2 est dans [0,1]
    float alpha = 0.5 * (1.0 + ndotL);
    vec3 goochTerm = mix(coolColor, warmColor, alpha);
    // On multiplie par l’intensité de la lumière pour garder la cohérence
    goochTerm *= light.intensities;

    // 2) Terme ambiant (classique)
    vec3 ambient = light.ambientCoefficient * light.intensities;

    // 3) Atténuation (optionnel)
    float dist = length(light.position - fragPosition);
    float attenuation = 1.0 / (1.0 + light.attenuation * dist * dist);

    // 4) Couleur finale
    vec3 color = (ambient + goochTerm) * materialAlbedo * attenuation;

    finalColor = vec4(color, 1.0);

}
