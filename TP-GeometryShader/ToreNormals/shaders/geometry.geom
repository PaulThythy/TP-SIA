#version 450

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in VS_OUT {
    vec3 worldPos;
    vec3 worldNormal;
} gs_in[];

uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;
uniform mat4 MVP;

const float NORMAL_LENGTH = 0.4;

void GenerateLine(int index) {
    vec3 startWorld = gs_in[index].worldPos;
    vec3 endWorld   = gs_in[index].worldPos + gs_in[index].worldNormal * NORMAL_LENGTH;
    gl_Position = PROJECTION * VIEW * vec4(startWorld, 1.0);
    EmitVertex();
    gl_Position = PROJECTION * VIEW * vec4(endWorld, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0);
}