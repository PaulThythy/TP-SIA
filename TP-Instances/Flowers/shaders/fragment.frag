#version 450

in vec2 UV;
flat in int TextureType;
flat in int HasFlower;

out vec4 FragColor;

uniform sampler2D grassTexture;
uniform sampler2D atlasTexture;

void main() {
    vec4 texColor;

    if (TextureType == 0) {
        texColor = texture(grassTexture, UV);
    } else {
        texColor = texture(atlasTexture, UV);
    }

    if (texColor.a < 0.1 || (texColor.r == 0.0 && texColor.g == 0.0 && texColor.b == 0.0)) {
        discard;
    }

    if (HasFlower == 1) {
        vec4 flowerColor = vec4(1.0, 0.5, 0.5, 1.0);
        texColor = mix(texColor, flowerColor, 0.3);
    }

    FragColor = texColor;
}
