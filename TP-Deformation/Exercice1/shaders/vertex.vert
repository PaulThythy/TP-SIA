#version 450

uniform mat4 MVP;
uniform mat4 MODEL;

uniform float thetaMax;

float z0 = -3;
float z1 = 3;

layout(location = 0) in vec3 position; 
layout(location = 2) in vec3 normal;

out vec3 fragPosition;
out vec3 fragNormal;

float r(float vertexCoord) {
    if(vertexCoord < z0) {
        return 0;
    }
    else if(vertexCoord <= z1 && vertexCoord >= z0) {
        return (vertexCoord-z0)/(z1-z0)*thetaMax;
    }
    else {
        return thetaMax;
    }
}

void main() {
    //[col][lig]
    mat3 rotationMatrix;
    rotationMatrix[0][0] = 1; rotationMatrix[1][0] = 0; rotationMatrix[2][0] = 0; 
    rotationMatrix[0][1] = 0; rotationMatrix[1][1] = cos(r(position.x)); rotationMatrix[2][1] = -sin(r(position.x)); 
    rotationMatrix[0][2] = 0; rotationMatrix[1][2] = sin(r(position.x)); rotationMatrix[2][2] = cos(r(position.x)); 

    vec3 twisted = rotationMatrix * position;
    vec4 posWorld = MODEL * vec4(twisted, 1.0);
    gl_Position  = MVP   * posWorld;
    fragPosition = posWorld.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(MODEL)));
    fragNormal = normalize(normalMatrix * normal);
}