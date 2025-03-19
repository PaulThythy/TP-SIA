/********************************************************/
/*                     CubeVBOShader.cpp                         */
/********************************************************/
/* Premiers pas avec OpenGL.                            */
/* Objectif : afficher a l'ecran uncube avec ou sans shader    */
/********************************************************/

/* inclusion des fichiers d'en-tete Glut */
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include "shader.hpp"
#include <string.h>
#include <vector>

// Include GLM
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

#include "ppm.h"
#include "tore.h"

// initialisations
void traceObjet(GLuint);
void loadShader(int);

// fonctions de rappel de glut
void display();
void affichageShadowMapShader();
void clavier(unsigned char, int, int);
void mouse(int, int, int, int);
void mouseMotion(int, int);
void reshape(int, int);

// variables globales pour OpenGL
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance = 0.;

// variables Handle d'opengl 
//--------------------------
GLuint MatrixIDMVP, MatrixIDView, MatrixIDModel, MatrixIDProjection; // handle pour la matrice MVP
GLuint locCameraPosition;
GLuint locmaterialShininess;
GLuint locmaterialSpecularColor;
GLuint locmaterialAlbedo;
GLuint locLightPosition;
GLuint locLightIntensities; //a.k.a the color of the light
GLuint locLightAttenuation;
GLuint locLightAmbientCoefficient;

int g_currentShaderIndex = 0;

//variable pour paramétrage eclairage
//--------------------------------------
glm::vec3 cameraPosition(0., 0., 3.);
// le matériau
//---------------
GLfloat materialShininess = 3.;
glm::vec3 materialSpecularColor(1., .1, 1); 
glm::vec3 materialAlbedo(1, 0, 0);                 // couleur du materiau

// la lumière
//-----------
glm::vec3 LightPosition(2., 4., 2.);
glm::vec3 LightIntensities(1., 1., 1.); // couleur la lumiere
GLfloat LightAttenuation = 0.05;
GLfloat LightAmbientCoefficient = .1;
glm::mat4 LightSpaceMatrix;

glm::mat4 MVP; // justement la voilà
glm::mat4 Model, View, Projection; // Matrices constituant MVP

int screenHeight = 500;
int screenWidth = 500;

// pour la map d'environnement 
//-------------------
GLuint bufSkybox;
GLuint locSkybox;

// pour la depth map
//-------------------
GLuint depthMapFBO;
GLuint depthMap;

GLuint shadow_width = 512;
GLuint shadow_height = 512;

Tore myTore;

void initSkybox(GLuint programID) 
{
  glGenTextures(1, &bufSkybox);
  glBindTexture(GL_TEXTURE_CUBE_MAP, bufSkybox);
  std::vector<std::string> faces = {
    "skybox/right.ppm",  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
    "skybox/left.ppm",   // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    "skybox/top.ppm",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    "skybox/bottom.ppm", // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    "skybox/front.ppm",  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    "skybox/back.ppm"    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  };

  for (int i = 0; i < 6; i++) {
    int w, h;
    GLubyte* data = glmReadPPM(const_cast<char*>(faces[i].c_str()), &w, &h);
    if (!data) {
        std::cerr << "Failed to load " << faces[i] << std::endl;
        continue;
    }

    // On transfère l’image dans la face i de la cube map
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0,              // mipmap level
        GL_RGB,         // format interne
        w, h,           // dimensions
        0,              // border
        GL_RGB,         // format source
        GL_UNSIGNED_BYTE,// type
        data            // pixels
    );

    // Libération de l’image en RAM (si vous n’en avez plus besoin)
    delete[] data;
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, bufSkybox);

  locSkybox = glGetUniformLocation(programID, "skybox");
}

void initDepthMap() {
  glGenFramebuffers(1, &depthMapFBO);
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//----------------------------------------
void initOpenGL(GLuint programID)
//----------------------------------------
{
  glCullFace(GL_BACK); // on spécifie queil faut éliminer les face arriere
  glEnable(GL_CULL_FACE); // on active l'élimination des faces qui par défaut n'est pas active
  glEnable(GL_DEPTH_TEST);

  // Get  handles for our matrix transformations "MVP" VIEW  MODELuniform
  MatrixIDMVP = glGetUniformLocation(programID, "MVP");
  MatrixIDView = glGetUniformLocation(programID, "VIEW");
  MatrixIDModel = glGetUniformLocation(programID, "MODEL");
  MatrixIDProjection = glGetUniformLocation(programID, "PROJECTION");

  // Projection matrix : 65 Field of View, 1:1 ratio, display range : 1 unit <-> 1000 units
  // ATTENTIOn l'angle est donné en radians si f GLM_FORCE_RADIANS est défini sinon en degré
  Projection = glm::perspective(glm::radians(60.f), 1.0f, 1.0f, 1000.0f);

  /* on recupere l'ID */
  locCameraPosition = glGetUniformLocation(programID, "cameraPosition");

  locmaterialShininess = glGetUniformLocation(programID, "material.shininess");
  locmaterialSpecularColor = glGetUniformLocation(programID, "material.specularColor");
  locmaterialAlbedo = glGetUniformLocation(programID, "material.albedo");
  locLightPosition = glGetUniformLocation(programID, "light.position");
  locLightIntensities = glGetUniformLocation(programID, "light.intensities");//a.k.a the color of the light
  locLightAttenuation = glGetUniformLocation(programID, "light.attenuation");
  locLightAmbientCoefficient = glGetUniformLocation(programID, "light.ambientCoefficient");

}
//----------------------------------------
int main(int argc, char ** argv)
//----------------------------------------
{

  /* initialisation de glut et creation de la fenetre */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(screenWidth, screenHeight);
  glutCreateWindow("EXERCICES SHADERS");

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  //info version GLSL
  std::cout << "***** Info GPU *****" << std::endl;
  std::cout << "Fabricant : " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Carte graphique: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Version GLSL : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;

  g_currentShaderIndex = 0;
  loadShader(g_currentShaderIndex);

  auto &sd = g_allShaders[g_currentShaderIndex];
  initOpenGL(sd.programID1);

  initDepthMap();

  myTore.createTorus(1., .3);

  // construction des VBO a partir des tableaux du cube deja construit
  myTore.genereVBO();
  myTore.initTexture(sd.programID1, "texture/opengl.ppm");
  initSkybox(sd.programID1);

  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(display);
  glutKeyboardFunc(clavier);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);

  /* Entree dans la boucle principale glut */
  glutMainLoop();

  glDeleteProgram(sd.programID1);
  myTore.deleteVBO();
  return 0;
}


void display() {
  /* effacement de l'image avec la couleur de fond */
  /* Initialisation d'OpenGL */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearDepth(10.0f); // 0 is near, >0 is far
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(2.0);

  auto &sd = g_allShaders[g_currentShaderIndex];

  if (!sd.m_multipass) {
    glUseProgram(sd.programID1);

    View = glm::lookAt(cameraPosition, // Camera is at (0,0,3), in World Space
      glm::vec3(0, 0, 0), // and looks at the origin
      glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
    );
    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, glm::vec3(0, 0, cameraDistance));
    Model = glm::rotate(Model, glm::radians(cameraAngleX), glm::vec3(1, 0, 0));
    Model = glm::rotate(Model, glm::radians(cameraAngleY), glm::vec3(0, 1, 0));
    Model = glm::scale(Model, glm::vec3(.8, .8, .8));
    MVP = Projection * View * Model;
    traceObjet(sd.programID1); // trace VBO avec ou sans shader

    /* on force l'affichage du resultat */
    glutPostRedisplay();
    glutSwapBuffers();

  } else {
    
    if(sd.m_id == ShaderID::SHADOW_MAP) {
      //passe 1 : depuis la lumière
      glViewport(0, 0, shadow_width, shadow_height);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);

      glUseProgram(sd.programID1);

      glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);
      glm::mat4 lightView = glm::lookAt(LightPosition, glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,1.f,0.f));
      LightSpaceMatrix = lightProjection * lightView;

      GLuint locLightSpaceMatrix = glGetUniformLocation(sd.programID1, "lightSpaceMatrix");
      glUniformMatrix4fv(locLightSpaceMatrix, 1, GL_FALSE, &LightSpaceMatrix[0][0]);

      traceObjet(sd.programID1);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      //passe 2 : depuis la caméra
      glViewport(0, 0, screenWidth, screenHeight);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(sd.programID2);

      View = glm::lookAt(cameraPosition, // Camera is at (0,0,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
      );
      Model = glm::mat4(1.0f);
      Model = glm::translate(Model, glm::vec3(0, 0, cameraDistance));
      Model = glm::rotate(Model, glm::radians(cameraAngleX), glm::vec3(1, 0, 0));
      Model = glm::rotate(Model, glm::radians(cameraAngleY), glm::vec3(0, 1, 0));
      Model = glm::scale(Model, glm::vec3(.8, .8, .8));
      MVP = Projection * View * Model;

      LightSpaceMatrix = lightProjection * lightView;

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      GLint locShadowMap = glGetUniformLocation(sd.programID2, "shadowMap");
      glUniform1i(locShadowMap, 0);

      glUniformMatrix4fv(locLightSpaceMatrix, 1, GL_FALSE, &LightSpaceMatrix[0][0]);

      traceObjet(sd.programID2);

      glutSwapBuffers();
    
    } else if(sd.m_id == ShaderID::TOON_OUTLINES_MULTIPASS || sd.m_id == ShaderID::GOOCH_OUTLINES_MULTIPASS) {
      //--------------------------------
      // PASSE 1 : Outline noir gonflé
      //--------------------------------
      glViewport(0, 0, screenWidth, screenHeight);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glCullFace(GL_FRONT);

      View  = glm::lookAt(cameraPosition, glm::vec3(0), glm::vec3(0,1,0));
      Model = glm::mat4(1.0f);
      Model = glm::translate(Model, glm::vec3(0, 0, cameraDistance));
      Model = glm::rotate(Model, glm::radians(cameraAngleX), glm::vec3(1,0,0));
      Model = glm::rotate(Model, glm::radians(cameraAngleY), glm::vec3(0,1,0));
      Model = glm::scale(Model, glm::vec3(0.8f));
      MVP   = Projection * View * Model;

      traceObjet(sd.programID1);

      //--------------------------------
      // PASSE 2 : Toon normal
      //--------------------------------
      glCullFace(GL_BACK); // On revient au cull "classique"

      traceObjet(sd.programID2);

      glutSwapBuffers();
    }
  }
}

//-------------------------------------
//Trace le tore 2 via le VAO
void traceObjet(GLuint programID)
//-------------------------------------
{
  // Use  shader & MVP matrix   MVP = Projection * View * Model;
  glUseProgram(programID);

  //on envoie les données necessaires aux shaders */
  glUniformMatrix4fv(MatrixIDMVP, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(MatrixIDView, 1, GL_FALSE,&View[0][0]);
  glUniformMatrix4fv(MatrixIDModel, 1, GL_FALSE, &Model[0][0]);
  glUniformMatrix4fv(MatrixIDProjection, 1, GL_FALSE, &Projection[0][0]);

  glUniform3f(locCameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);

  glUniform1f(locmaterialShininess,materialShininess);
  glUniform3f(locmaterialSpecularColor,materialSpecularColor.x,materialSpecularColor.y,materialSpecularColor.z);
  glUniform3f(locmaterialAlbedo, materialAlbedo.x, materialAlbedo.y, materialAlbedo.z);
  glUniform3f(locLightPosition,LightPosition.x,LightPosition.y,LightPosition.z);
  glUniform3f(locLightIntensities,LightIntensities.x,LightIntensities.y,LightIntensities.z);
  glUniform1f(locLightAttenuation,LightAttenuation);
  glUniform1f(locLightAmbientCoefficient,LightAmbientCoefficient);


  //pour l'affichage
  glBindVertexArray(myTore.VAO);                                              // on active le VAO
  glDrawElements(GL_TRIANGLES, sizeof(myTore.indices), GL_UNSIGNED_INT, 0);   // on appelle la fonction dessin 
  glBindVertexArray(0);                                                       // on desactive les VAO
  glUseProgram(0);                                                            // et le pg

}

void reshape(int w, int h) {
  // set viewport to be the entire window
  glViewport(0, 0, (GLsizei) w, (GLsizei) h); // ATTENTION GLsizei important - indique qu'il faut convertir en entier non négatif

  // set perspective viewing frustum
  float aspectRatio = (float) w / h;

  Projection = glm::perspective(glm::radians(60.0f), (float)(w) / (float) h, 1.0f, 1000.0f);

  screenWidth = w; screenHeight = h;
}

void clavier(unsigned char touche, int x, int y) {
  switch (touche) {
  case 'f':
    /* affichage du carre plein */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glutPostRedisplay();
    break;
  case 'e':
    /* affichage en mode fil de fer */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutPostRedisplay();
    break;
  case 'v':
    /* Affichage en mode sommets seuls */
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glutPostRedisplay();
    break;
  case 's':
    /* Affichage en mode sommets seuls */
    materialShininess -= .5;
    glutPostRedisplay();
    break;
  case 'S':
    /* Affichage en mode sommets seuls */
    materialShininess += .5;
    glutPostRedisplay();
    break;
  case 'x':
    /* Affichage en mode sommets seuls */
    LightPosition.x -= .2;
    glutPostRedisplay();
    break;
  case 'X':
    /* Affichage en mode sommets seuls */
    LightPosition.x += .2;
    glutPostRedisplay();
    break;
  case 'y':
    /* Affichage en mode sommets seuls */
    LightPosition.y -= .2;
    glutPostRedisplay();
    break;
  case 'Y':
    /* Affichage en mode sommets seuls */
    LightPosition.y += .2;
    glutPostRedisplay();
    break;
  case 'z':
    /* Affichage en mode sommets seuls */
    LightPosition.z -= .2;
    glutPostRedisplay();
    break;
  case 'Z':
    /* Affichage en mode sommets seuls */
    LightPosition.z += .2;
    glutPostRedisplay();
    break;
  case 'a':
    /* Affichage en mode sommets seuls */
    LightAmbientCoefficient -= .1;
    glutPostRedisplay();
    break;
  case 'A':
    /* Affichage en mode sommets seuls */
    LightAmbientCoefficient += .1;
    glutPostRedisplay();
    break;
  case 'r':
    /* reload le shader */
    g_currentShaderIndex++;
    if (g_currentShaderIndex >= (int)g_allShaders.size()) {
        g_currentShaderIndex = 0; // on revient au premier
    }
    loadShader(g_currentShaderIndex);
    glutPostRedisplay();

    break;

  case 'q':
    /*la touche 'q' permet de quitter le programme */
    exit(0);
  }
}

void mouse(int button, int state, int x, int y) {
  mouseX = x;
  mouseY = y;

  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseLeftDown = true;
    } else if (state == GLUT_UP)
      mouseLeftDown = false;
  } else if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseRightDown = true;
    } else if (state == GLUT_UP)
      mouseRightDown = false;
  } else if (button == GLUT_MIDDLE_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseMiddleDown = true;
    } else if (state == GLUT_UP)
      mouseMiddleDown = false;
  }
}

void mouseMotion(int x, int y) {
  if (mouseLeftDown) {
    cameraAngleY += (x - mouseX);
    cameraAngleX += (y - mouseY);
    mouseX = x;
    mouseY = y;
  }
  if (mouseRightDown) {
    cameraDistance += (y - mouseY) * 0.2f;
    mouseY = y;
  }

  glutPostRedisplay();
}

void loadShader(int i)
{
    auto &sd = g_allShaders[i];

    if (glIsProgram(sd.programID1))
        glDeleteProgram(sd.programID1);
    if (glIsProgram(sd.programID2))
        glDeleteProgram(sd.programID2);

    // Compile la passe 1
    sd.programID1 = LoadShaders(sd.m_pass1Paths.vs.c_str(), sd.m_pass1Paths.fs.c_str());

    // Si multipasse, compile la passe 2
    if (sd.m_multipass) {
        sd.programID2 = LoadShaders(sd.m_pass2Paths.vs.c_str(), sd.m_pass2Paths.fs.c_str());
        initOpenGL(sd.programID2);
    } else {
        sd.programID2 = 0;
        initOpenGL(sd.programID1);
    }

    std::cout << "Shader chargé : " << sd.m_shaderName << std::endl;
}