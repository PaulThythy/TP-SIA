#version 450

layout(location = 0) in vec3 inPosition; 
layout(location = 1) in vec2 inUV;       
layout(location = 2) in vec3 inNormal;

// Uniforms
uniform mat4 MODEL;
uniform mat4 MVP;
float pixelSize = 0.17;

// Sorties vers le fragment shader
out vec3 fragNormal;
out vec2 fragUV;

void main()
{
    // 1) On calcule la position du sommet en espace monde
    //    (vous pouvez aussi faire la quantification en espace objet si vous préférez)
    vec4 worldPos4 = MODEL * vec4(inPosition, 1.0);
    vec3 worldPos  = worldPos4.xyz;

    // 2) On quantifie la position pour créer l'effet “pixélisé”.
    //    floor(x + 0.5) ~ arrondi à l’entier le plus proche. 
    //    Puis on multiplie par pixelSize pour revenir dans l'échelle initiale.
    vec3 quantizedPos = floor((worldPos / pixelSize) + 0.5) * pixelSize;

    // 3) On passe la position quantifiée au pipeline :
    gl_Position = MVP * vec4(quantizedPos, 1.0);

    // 4) On transmet la normale et les UV tels quels (ou on pourrait aussi “pixéliser” la normale).
    fragNormal = inNormal; 
    fragUV     = inUV;
}
