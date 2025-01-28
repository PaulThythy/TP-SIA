#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

out VS_OUT {
    vec3 position;
    vec3 normal;
} vs_out;

void main(){
    vs_out.position = vec3(MODEL * vec4(vertexPosition, 1.0));
    vs_out.normal = normalize(mat3(MODEL) * vertexNormal);
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}
