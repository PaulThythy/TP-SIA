#version 450

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform mat4 MVP;

uniform sampler2D textureSampler;

in vec2 UV;
out vec4 finalColor;

void main() {
    vec4 texColor = texture(textureSampler, UV);
    if(texColor.a < 0.1 || (texColor.r == 0.0 && texColor.g == 0.0 && texColor.b == 0.0))
        discard;

    finalColor = texColor;
}