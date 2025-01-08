#ifndef TORE_H
#define TORE_H

#include <GL/glew.h>

#include "../glm/glm.hpp"
#include "ppm.h"

#define P_SIZE 3
#define N_SIZE 3 // c'est forcement 3
#define C_SIZE 3

#define N_VERTS 8
#define N_VERTS_BY_FACE 3
#define N_FACES 12

#define NB_R 40
#define NB_r 20
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct Tore
{
    GLfloat sommets[(NB_R + 1) * (NB_r + 1) * 3];      // x 3 coordonnées (+1 acr on double les dernierspoints pour avoir des coord de textures <> pour les points de jonctions)
    GLuint indices[NB_R * NB_r * 6];                   // x6 car pour chaque face quadrangulaire on a 6 indices (2 triangles=2x 3 indices)
    GLfloat coordTexture[(NB_R + 1) * (NB_r + 1) * 2]; // x 2 car U+V par sommets
    GLfloat normales[(NB_R + 1) * (NB_r + 1) * 3];

    GLuint VBO_sommets, VBO_normales, VBO_indices, VBO_UVtext, VAO;

    GLuint indexVertex = 0, indexUVTexture = 1, indexNormale = 2;

    GLuint image;
    GLuint bufTexture, bufNormalMap;
    GLuint locationTexture, locationNormalMap;

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

    inline void genereVBO()
    {
        if (glIsBuffer(VBO_sommets) == GL_TRUE)
            glDeleteBuffers(1, &VBO_sommets);
        glGenBuffers(1, &VBO_sommets);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_sommets);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sommets), sommets, GL_STATIC_DRAW);

        if (glIsBuffer(VBO_normales) == GL_TRUE)
            glDeleteBuffers(1, &VBO_normales);
        glGenBuffers(1, &VBO_normales);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normales);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normales), normales, GL_STATIC_DRAW);

        if (glIsBuffer(VBO_indices) == GL_TRUE)
            glDeleteBuffers(1, &VBO_indices);
        glGenBuffers(1, &VBO_indices); // ATTENTIOn IBO doit etre un GL_ELEMENT_ARRAY_BUFFER
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        if (glIsBuffer(VBO_UVtext) == GL_TRUE)
            glDeleteBuffers(1, &VBO_UVtext);
        glGenBuffers(1, &VBO_UVtext);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UVtext);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordTexture), coordTexture, GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO); // ici on bind le VAO , c'est lui qui recupèrera les configurations des VBO glVertexAttribPointer , glEnableVertexAttribArray...
        glEnableVertexAttribArray(indexVertex);
        glEnableVertexAttribArray(indexNormale);
        glEnableVertexAttribArray(indexUVTexture);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_sommets);
        glVertexAttribPointer(indexVertex, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_normales);
        glVertexAttribPointer(indexNormale, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_UVtext);
        glVertexAttribPointer(indexUVTexture, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indices);

        // une fois la config terminée
        // on désactive le dernier VBO et le VAO pour qu'ils ne soit pas accidentellement modifié
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    inline void deleteVBO()
    {
        glDeleteBuffers(1, &VBO_sommets);
        glDeleteBuffers(1, &VBO_normales);
        glDeleteBuffers(1, &VBO_indices);
        glDeleteBuffers(1, &VBO_UVtext);
        glDeleteBuffers(1, &VAO);
    }

    inline void initTexture(GLuint programID, char* texturePath) {
        int iwidth, iheight;
        GLubyte * image = NULL;

        image = glmReadPPM(texturePath, &iwidth, &iheight);
        glGenTextures(1, &bufTexture);
        glBindTexture(GL_TEXTURE_2D, bufTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

        locationTexture = glGetUniformLocation(programID, "textureSampler"); // et il y a la texture elle même  
        glBindAttribLocation(programID,indexUVTexture,"vertexUV");	// il y a les coord UV  
    }
};

#endif