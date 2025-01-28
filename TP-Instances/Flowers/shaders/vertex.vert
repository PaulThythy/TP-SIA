#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

out vec2 UV;

void main(){
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    UV = vertexUV;
}