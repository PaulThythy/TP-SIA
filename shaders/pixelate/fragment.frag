#version 450

in vec3 fragNormal;
// Supposez que vous ayez un uniform color ou materialAlbedo

out vec4 finalColor;

vec3 color = vec3(1.0, 0.0, 0.0); 

void main()
{
    // Lambert simplifié avec une lumière directionnelle
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(vec3(0.4, 1.0, 0.2)); // direction fixe pour la démo

    float diffuse = max(dot(N, L), 0.0);
    finalColor = vec4(color * diffuse, 1.0);
}
