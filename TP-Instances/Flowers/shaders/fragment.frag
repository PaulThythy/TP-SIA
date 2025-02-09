#version 450

in vec2 UV;
flat in int TextureType;
flat in int HasFlower;
flat in int instanceVariant;

out vec4 FragColor;

uniform sampler2D grassTexture;
uniform sampler2D atlasTexture;

void main() {
    float flippedY = 1.0 - UV.y;
    vec4 texColor;

    if (TextureType == 0) {
        texColor = texture(grassTexture, vec2(UV.x, flippedY));
    } else {
        vec2 uvGrass = vec2(
            UV.x * 0.25 + float(instanceVariant) * 0.25,
            flippedY * 0.5 + 0.5
        );
        texColor = texture(atlasTexture, uvGrass);

        if (HasFlower == 1) {
            vec2 uvFlower = vec2(
                UV.x * 0.25 + float(instanceVariant) * 0.25,
                flippedY * 0.5
            );
            vec4 flowerColor = texture(atlasTexture, uvFlower);
            flowerColor.rgb *= 4.5;
            texColor = mix(texColor, flowerColor, 0.3);
        }
    }

    if (texColor.a < 0.1 || (texColor.r == 0.0 && texColor.g == 0.0 && texColor.b == 0.0)) {
        discard;
    } 

    FragColor = texColor;
}
