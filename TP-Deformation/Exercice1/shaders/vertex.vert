#version 450

uniform mat4 MVP;
uniform mat4 MODEL;

uniform float thetaMax;

float coordMin = -3;
float coordMax = 3;

float EPSILON = 1e-4;

layout(location = 0) in vec3 position; 
layout(location = 2) in vec3 normal;

out vec3 fragPosition;
out vec3 fragNormal;

float r(float vertexCoord) {
    if(vertexCoord < coordMin) {
        return 0;
    }
    else if(vertexCoord <= coordMax && vertexCoord >= coordMin) {
        return (vertexCoord-coordMin)/(coordMax-coordMin)*thetaMax;
    }
    else {
        return thetaMax;
    }
}

vec3 twistPos(in vec3 p) {
    float angle = r(p.x);
    float c = cos(angle);
    float s = sin(angle);

    mat3 twistM = mat3(0.0);
    twistM[0][0] = 1.0; 
    twistM[1][1] = c;
    twistM[2][1] = -s;

    twistM[1][2] = s;
    twistM[2][2] = c;

    return twistM * p;
}

vec3 finiteDifferences(in vec3 p, in vec3 e, float eps) {
    return (twistPos(p + e) - twistPos(p)) / eps;
}

void main() {
    vec3 twistedLocal = twistPos(position);
    vec4 posWorld = MODEL * vec4(twistedLocal, 1.0);
    gl_Position  = MVP * posWorld;
    fragPosition = posWorld.xyz;

    vec3 dFdx = finiteDifferences(position, vec3(EPSILON, 0.0, 0.0), EPSILON);
    vec3 dFdy = finiteDifferences(position, vec3(0.0, EPSILON, 0.0), EPSILON);
    vec3 dFdz = finiteDifferences(position, vec3(0.0, 0.0, EPSILON), EPSILON);

    // Construction de la matrice jacobienne J (colonnes : dF/dx, dF/dy, dF/dz)
    mat3 J = mat3(dFdx, dFdy, dFdz);

    // Transformation de la normale via la jacobienne locale
    vec3 twistedNormalLocal = inverse(transpose(J)) * normal;

    // Transformation des normales par MODEL
    mat3 normalMatrix = transpose(inverse(mat3(MODEL)));
    fragNormal = normalize(normalMatrix * twistedNormalLocal);
}