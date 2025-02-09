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
#include <cstddef>

// Include GLM
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// fonctions de rappel de glut
void affichage();
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
GLuint programID;                                                    // handle pour le shader
GLuint MatrixIDMVP, MatrixIDView, MatrixIDModel, MatrixIDProjection; // handle pour la matrice MVP
GLuint locCameraPosition;
GLuint locmaterialShininess;
GLuint locmaterialSpecularColor;
GLuint locmaterialAlbedo;
GLuint locLightPosition;
GLuint locLightIntensities; // a.k.a the color of the light
GLuint locLightAttenuation;
GLuint locLightAmbientCoefficient;

GLuint image;
GLuint bufTexture, bufNormalMap;
GLuint locTexture, locNormalMap;

// variable pour paramétrage eclairage
//--------------------------------------
glm::vec3 cameraPosition(0., 0., 3.);
// le matériau
//---------------
GLfloat materialShininess = 3.;
glm::vec3 materialSpecularColor(1., .1, 1);
glm::vec3 materialAlbedo(1, 1, 1); // couleur du materiau

// la lumière
//-----------
glm::vec3 LightPosition(2., 4., 2.);
glm::vec3 LightIntensities(1., 1., 1.); // couleur la lumiere
GLfloat LightAttenuation = 1.;
GLfloat LightAmbientCoefficient = .1;

glm::mat4 MVP;                     // justement la voilà
glm::mat4 Model, View, Projection; // Matrices constituant MVP

int screenHeight = 500;
int screenWidth = 500;

struct InstanceData
{
  glm::vec3 position;
  float scale;
  float rotation;
  int textureType; // 0: simple grass, 1: atlas
  int hasFlower;   // 0: no flower, 1: with flower
};

GLuint vao, vbo, instanceVBO;
GLuint grassTextureID, atlasTextureID;
std::vector<InstanceData> instances;
const int numInstances = 1000;

GLuint loadTexture(const char *filepath)
{
  stbi_set_flip_vertically_on_load(true);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  int width, height, nrChannels;
  unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cerr << "Failed to load texture: " << filepath << std::endl;
  }
  stbi_image_free(data);
  return texture;
}

void generateInstances()
{
  srand(static_cast<unsigned>(time(0)));
  for (int i = 0; i < numInstances; ++i)
  {
    InstanceData instance;
    instance.position = glm::vec3(
        (rand() % 200 - 100) / 10.0f, // Position X
        0.0f,                         // Position Y
        (rand() % 200 - 100) / 10.0f  // Position Z
    );
    instance.scale = (rand() % 50 + 50) / 100.0f;                      // Scale entre 0.5 et 1.0
    instance.rotation = (rand() % 360) * glm::radians(1.0f);           // Rotation en radians
    instance.textureType = rand() % 2;                                 // 0 ou 1 (herbe simple ou atlas)
    instance.hasFlower = (instance.textureType == 1) ? rand() % 2 : 0; // Fleurs uniquement avec atlas
    instances.push_back(instance);
  }
}

void initBuffers()
{
  float halfX = 0.5f;
  float halfY = 0.5f;

  float c60 = cos(glm::radians(60.0f));
  float s60 = sin(glm::radians(60.0f));

  float c120 = cos(glm::radians(120.0f));
  float s120 = sin(glm::radians(120.0f));

  float vertices[] = {
      // ----------------- Quad 1 : 0°  -----------------
      -halfX, -halfY, 0.0f, 0.0f, 1.0f, // bas gauche
      halfX, -halfY, 0.0f, 1.0f, 1.0f,  // bas droite
      halfX, halfY, 0.0f, 1.0f, 0.0f,   // haut droite
      -halfX, halfY, 0.0f, 0.0f, 0.0f,  // haut gauche

      // ----------------- Quad 2 : +60°  --------------
      // On applique la rotation (x, z) -> (x*c60 - z*s60, x*s60 + z*c60)
      (-halfX * c60) - (0.0f * s60), -halfY, (-halfX * s60) + (0.0f * c60), 0.0f, 1.0f,
      (halfX * c60) - (0.0f * s60), -halfY, (halfX * s60) + (0.0f * c60), 1.0f, 1.0f,
      (halfX * c60) - (0.0f * s60), halfY, (halfX * s60) + (0.0f * c60), 1.0f, 0.0f,
      (-halfX * c60) - (0.0f * s60), halfY, (-halfX * s60) + (0.0f * c60), 0.0f, 0.0f,

      // ----------------- Quad 3 : +120° --------------
      (-halfX * c120) - (0.0f * s120), -halfY, (-halfX * s120) + (0.0f * c120), 0.0f, 1.0f,
      (halfX * c120) - (0.0f * s120), -halfY, (halfX * s120) + (0.0f * c120), 1.0f, 1.0f,
      (halfX * c120) - (0.0f * s120), halfY, (halfX * s120) + (0.0f * c120), 1.0f, 0.0f,
      (-halfX * c120) - (0.0f * s120), halfY, (-halfX * s120) + (0.0f * c120), 0.0f, 0.0f};

  // Création et liaison du VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Création du VBO pour les sommets
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Attribut 0 : positions (vec3)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Attribut 1 : coordonnées UV (vec2)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Création du buffer pour les données d'instance
  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData), instances.data(), GL_STATIC_DRAW);

  // Attribut 2 : position de l'instance (vec3)
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void *)offsetof(InstanceData, position));
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);

  // Attribut 3 : échelle de l'instance (float)
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void *)offsetof(InstanceData, scale));
  glEnableVertexAttribArray(3);
  glVertexAttribDivisor(3, 1);

  // Attribut 4 : rotation de l'instance (float)
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void *)offsetof(InstanceData, rotation));
  glEnableVertexAttribArray(4);
  glVertexAttribDivisor(4, 1);

  // Attribut 5 : type de texture (int)
  glVertexAttribIPointer(5, 1, GL_INT, sizeof(InstanceData), (void *)offsetof(InstanceData, textureType));
  glEnableVertexAttribArray(5);
  glVertexAttribDivisor(5, 1);

  // Attribut 6 : présence de fleurs (int)
  glVertexAttribIPointer(6, 1, GL_INT, sizeof(InstanceData), (void *)offsetof(InstanceData, hasFlower));
  glEnableVertexAttribArray(6);
  glVertexAttribDivisor(6, 1);
}

//----------------------------------------
void initOpenGL(GLuint programID)
//----------------------------------------
{
  // glCullFace(GL_BACK);    // on spécifie queil faut éliminer les face arriere
  // glEnable(GL_CULL_FACE); // on active l'élimination des faces qui par défaut n'est pas active
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
  locLightIntensities = glGetUniformLocation(programID, "light.intensities"); // a.k.a the color of the light
  locLightAttenuation = glGetUniformLocation(programID, "light.attenuation");
  locLightAmbientCoefficient = glGetUniformLocation(programID, "light.ambientCoefficient");
}
//----------------------------------------
int main(int argc, char **argv)
//----------------------------------------
{

  /* initialisation de glut et creation de la fenetre */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(screenWidth, screenHeight);
  glutCreateWindow("EXERCICES INSTANCES");

  // Initialize GLEW
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  // info version GLSL
  std::cout << "***** Info GPU *****" << std::endl;
  std::cout << "Fabricant : " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Carte graphique: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Version GLSL : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl
            << std::endl;

  programID = LoadShaders("shaders/vertex.vert", "shaders/fragment.frag");
  initOpenGL(programID);

  grassTextureID = loadTexture("textures/s_grass.png");
  atlasTextureID = loadTexture("textures/s_grass_atlas.png");
  generateInstances();
  initBuffers();

  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);

  /* Entree dans la boucle principale glut */
  glutMainLoop();

  glDeleteProgram(programID);
  return 0;
}

/* fonction d'affichage */
void affichage()
{

  /* effacement de l'image avec la couleur de fond */
  /* Initialisation d'OpenGL */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearDepth(10.0f); // 0 is near, >0 is far
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(2.0);

  View = glm::lookAt(cameraPosition,     // Camera is at (0,0,3), in World Space
                     glm::vec3(0, 0, 0), // and looks at the origin
                     glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );
  Model = glm::mat4(1.0f);
  Model = glm::translate(Model, glm::vec3(0, 0, cameraDistance));
  Model = glm::rotate(Model, glm::radians(cameraAngleX), glm::vec3(1, 0, 0));
  Model = glm::rotate(Model, glm::radians(cameraAngleY), glm::vec3(0, 1, 0));
  Model = glm::scale(Model, glm::vec3(.8, .8, .8));
  MVP = Projection * View * Model;

  glUseProgram(programID);

  // update uniforms
  glUniformMatrix4fv(MatrixIDMVP, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(MatrixIDView, 1, GL_FALSE, &View[0][0]);
  glUniformMatrix4fv(MatrixIDModel, 1, GL_FALSE, &Model[0][0]);
  glUniformMatrix4fv(MatrixIDProjection, 1, GL_FALSE, &Projection[0][0]);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, grassTextureID);
  glUniform1i(glGetUniformLocation(programID, "grassTexture"), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, atlasTextureID);
  glUniform1i(glGetUniformLocation(programID, "atlasTexture"), 1);

  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_QUADS, 0, 12, numInstances);

  /* on force l'affichage du resultat */
  glutPostRedisplay();
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  // set viewport to be the entire window
  glViewport(0, 0, (GLsizei)w, (GLsizei)h); // ATTENTION GLsizei important - indique qu'il faut convertir en entier non négatif

  // set perspective viewing frustum
  float aspectRatio = (float)w / h;

  Projection = glm::perspective(glm::radians(60.0f), (float)(w) / (float)h, 1.0f, 1000.0f);

  screenWidth = w;
  screenHeight = h;
}

void clavier(unsigned char touche, int x, int y)
{
  switch (touche)
  {
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
    // TODO refresh flowers to have a new repartition

  case 'q':
    /*la touche 'q' permet de quitter le programme */
    exit(0);
  }
}

void mouse(int button, int state, int x, int y)
{
  mouseX = x;
  mouseY = y;

  if (button == GLUT_LEFT_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseLeftDown = true;
    }
    else if (state == GLUT_UP)
      mouseLeftDown = false;
  }
  else if (button == GLUT_RIGHT_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseRightDown = true;
    }
    else if (state == GLUT_UP)
      mouseRightDown = false;
  }
  else if (button == GLUT_MIDDLE_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseMiddleDown = true;
    }
    else if (state == GLUT_UP)
      mouseMiddleDown = false;
  }
}

void mouseMotion(int x, int y)
{
  if (mouseLeftDown)
  {
    cameraAngleY += (x - mouseX);
    cameraAngleX += (y - mouseY);
    mouseX = x;
    mouseY = y;
  }
  if (mouseRightDown)
  {
    cameraDistance += (y - mouseY) * 0.2f;
    mouseY = y;
  }

  glutPostRedisplay();
}