#version 450

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform mat4 MVP;

uniform int textureIndex;
uniform sampler2D textureSampler;

in vec2 UV;
out vec4 finalColor;

void main() {
    const int ATLAS_COLS = 4;
    const int ATLAS_ROWS = 2;

    float u = UV.x / ATLAS_COLS + (textureIndex % ATLAS_COLS) * (1.0 / ATLAS_COLS);
    float v = UV.y / ATLAS_ROWS + (textureIndex / ATLAS_COLS) * (1.0 / ATLAS_ROWS);

    vec4 textureColor = texture(textureSampler, vec2(u, v));

    if(texColor.a < 0.1)
        discard;

    finalColor = textureColor;
}