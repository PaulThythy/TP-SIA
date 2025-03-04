#ifndef TORE_H
#define TORE_H

#include <GL/glew.h>

#include "../glm/glm.hpp"

#define P_SIZE 3
#define N_SIZE 3 // c'est forcement 3
#define C_SIZE 3

#define N_VERTS 8
#define N_VERTS_BY_FACE 3
#define N_FACES 12

#define NB_R 40
#define NB_r 20
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct VertexData {
    GLfloat position[3];
    GLfloat uv[2];
    GLfloat normal[3];
};

struct Tore
{
    GLfloat sommets[(NB_R + 1) * (NB_r + 1) * 3];      // x 3 coordonnées (+1 acr on double les dernierspoints pour avoir des coord de textures <> pour les points de jonctions)
    GLuint indices[NB_R * NB_r * 6];                   // x6 car pour chaque face quadrangulaire on a 6 indices (2 triangles=2x 3 indices)
    GLfloat coordTexture[(NB_R + 1) * (NB_r + 1) * 2]; // x 2 car U+V par sommets
    GLfloat normales[(NB_R + 1) * (NB_r + 1) * 3];

    GLuint tore_SSBO;

    inline void createTorus(float R, float r)
    {
        float theta, phi;
        theta = ((float)glm::radians(360.f)) / ((float)NB_R);
        phi = ((float)(glm::radians(360.f))) / ((float)NB_r);

        float pasU, pasV;
        pasU = 1. / NB_R;
        pasV = 1. / NB_r;
        for (int i = 0; i <= NB_R; i++)
            for (int j = 0; j <= NB_r; j++)
            {
                float a, b, c;
                sommets[(i * (NB_r + 1) * 3) + (j * 3)] = (R + r * cos((float)j * phi)) * cos((float)i * theta);     // x
                sommets[(i * (NB_r + 1) * 3) + (j * 3) + 1] = (R + r * cos((float)j * phi)) * sin((float)i * theta); // y
                sommets[(i * (NB_r + 1) * 3) + (j * 3) + 2] = r * sin((float)j * phi);

                normales[(i * (NB_r + 1) * 3) + (j * 3)] = cos((float)j * phi) * cos((float)i * theta);     // x
                normales[(i * (NB_r + 1) * 3) + (j * 3) + 1] = cos((float)j * phi) * sin((float)i * theta); // y
                normales[(i * (NB_r + 1) * 3) + (j * 3) + 2] = sin((float)j * phi);

                coordTexture[(i * (NB_r + 1) * 2) + (j * 2)] = ((float)i) * pasV;
                coordTexture[(i * (NB_r + 1) * 2) + (j * 2) + 1] = ((float)j) * pasV;
            }

        int indiceMaxI = ((NB_R + 1) * (NB_r)) - 1;
        int indiceMaxJ = (NB_r + 1);

        for (int i = 0; i < NB_R; i++)
            for (int j = 0; j < NB_r; j++)
            {
                int i0, i1, i2, i3, i4, i5;
                indices[(i * NB_r * 6) + (j * 6)] = (unsigned int)((i * (NB_r + 1)) + j);
                indices[(i * NB_r * 6) + (j * 6) + 1] = (unsigned int)((i + 1) * (NB_r + 1) + (j));
                indices[(i * NB_r * 6) + (j * 6) + 2] = (unsigned int)(((i + 1) * (NB_r + 1)) + (j + 1));
                indices[(i * NB_r * 6) + (j * 6) + 3] = (unsigned int)((i * (NB_r + 1)) + j);
                indices[(i * NB_r * 6) + (j * 6) + 4] = (unsigned int)(((i + 1) * (NB_r + 1)) + (j + 1));
                indices[(i * NB_r * 6) + (j * 6) + 5] = (unsigned int)(((i) * (NB_r + 1)) + (j + 1));
            }
    }

    // Construction du SSBO intercalé à partir des tableaux et indices
    inline void genereSSBO()
    {
        // Le nombre de sommets à dessiner est égal au nombre d'indices
        int nbVertices = NB_R * NB_r * 6;
        std::vector<VertexData> buffer(nbVertices);

        for (int i = 0; i < nbVertices; i++)
        {
            // L'indice dans les tableaux de base
            GLuint idx = indices[i];
            // Positions
            buffer[i].position[0] = sommets[idx * 3];
            buffer[i].position[1] = sommets[idx * 3 + 1];
            buffer[i].position[2] = sommets[idx * 3 + 2];
            // Coordonnées de texture
            buffer[i].uv[0] = coordTexture[idx * 2];
            buffer[i].uv[1] = coordTexture[idx * 2 + 1];
            // Normales
            buffer[i].normal[0] = normales[idx * 3];
            buffer[i].normal[1] = normales[idx * 3 + 1];
            buffer[i].normal[2] = normales[idx * 3 + 2];
        }
        // Création et remplissage du SSBO
        glGenBuffers(1, &tore_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, tore_SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, buffer.size() * sizeof(VertexData), buffer.data(), GL_STATIC_DRAW);
        // On lie le buffer au binding point 0
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, tore_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // Suppression du SSBO
    inline void deleteSSBO()
    {
        glDeleteBuffers(1, &tore_SSBO);
    }
};

#endif