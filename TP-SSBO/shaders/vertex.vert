#version 450

// Déclaration de la structure et du SSBO intercalé
struct VertexData {
    float position[3];
    float uv[2];
    float normal[3];
};

layout(binding = 0, std430) readonly buffer ssbo1 {
    VertexData data[];
};

vec3 getPosition(int index) {
  return vec3(
    data[index].position[0],
    data[index].position[1],
    data[index].position[2]
  );
}

vec2 getUV(int index) {
  return vec2(
    data[index].uv[0],
    data[index].uv[1]
  );
}

vec3 getNormal(int index) {
  return vec3(
    data[index].normal[0],
    data[index].normal[1],
    data[index].normal[2]
  );
}

uniform mat4 MVP;
uniform mat4 MODEL;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragUV;

void main() {
    vec3 pos = getPosition(gl_VertexID);
    gl_Position = MVP * vec4(pos, 1.0);
    
    vec4 posWorld = MODEL * vec4(pos, 1.0);
    fragPosition = posWorld.xyz;
    
    vec3 norm = getNormal(gl_VertexID);
    mat3 normalMatrix = transpose(inverse(mat3(MODEL)));
    fragNormal = normalize(normalMatrix * norm);
    
    fragUV = getUV(gl_VertexID);
}
