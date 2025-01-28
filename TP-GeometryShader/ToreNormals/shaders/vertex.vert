#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

out VS_OUT {
    vec3 normal;
} vs_out;

void main(){
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(VIEW * MODEL)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * vertexNormal, 0.0)));
}
