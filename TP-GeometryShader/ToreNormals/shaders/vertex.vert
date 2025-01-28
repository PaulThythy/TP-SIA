#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

out VS_OUT {
    vec3 worldPos;
    vec3 worldNormal;
} vs_out;

void main(){
    vec4 posWorld4 = MODEL * vec4(vertexPosition, 1.0);
    vs_out.worldPos = posWorld4.xyz;
    mat3 normalMatrix = mat3(transpose(inverse(MODEL)));
    vs_out.worldNormal = normalize(normalMatrix * vertexNormal);
    gl_Position = PROJECTION * VIEW * posWorld4;
}
