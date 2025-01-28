#version 450

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in VS_OUT{
    vec3 normal;
} gs_in[];

uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;
uniform mat4 MVP;

const float NORMAL_LENGTH = 0.4;

void GenerateLine(int index) {
    gl_Position = PROJECTION * gl_in[index].gl_Position;
    EmitVertex();
    
    gl_Position = PROJECTION * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * NORMAL_LENGTH);
    EmitVertex();
    
    EndPrimitive();
}

void main() {
    GenerateLine(0);
}