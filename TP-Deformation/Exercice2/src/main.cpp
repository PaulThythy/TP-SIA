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
#include <cmath>
#include "shader.hpp"
#include <string.h>
#include <vector>

// Include GLM
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

// initialisations
void traceObjet();

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

GLuint vbo_circle_position, vbo_circle_colors, vao_circle;
int circleResolution = 100;
int selectedPointIndex = 0;

GLuint vbo_bezier_surf_position, vbo_bezier_surf_colors, vao_surf_bezier;

std::vector<GLfloat> squarePositions = {
  -1.f, -1.f,  0.f,   
   1.f, -1.f,  0.f,   
   1.f,  1.f,  0.f,  
  -1.f,  1.f,  0.f 
};

// variable pour paramétrage eclairage
//--------------------------------------
glm::vec3 cameraPosition(0., 0., 3.);
// le matériau
//---------------
GLfloat materialShininess = 3.;
glm::vec3 materialSpecularColor(1., .1, 1);
glm::vec3 materialAlbedo(1, 0, 0); // couleur du materiau

// la lumière
//-----------
glm::vec3 LightPosition(2., 4., 2.);
glm::vec3 LightIntensities(1., 1., 1.); // couleur la lumiere
GLfloat LightAttenuation = .001;
GLfloat LightAmbientCoefficient = .1;

glm::mat4 MVP;                     // justement la voilà
glm::mat4 Model, View, Projection; // Matrices constituant MVP

int screenHeight = 500;
int screenWidth = 500;

void initBuffers()
{
  //cercle unitaire
  std::vector<GLfloat> circlePositions;
  std::vector<GLfloat> circleColors;
  circlePositions.reserve(circleResolution * 3);
  circleColors.reserve(circleResolution * 3);

  float angleStep = 2.0f * float(M_PI) / circleResolution;
  for (int i = 0; i < circleResolution; ++i)
  {
    float angle = i * angleStep;
    float x = std::cos(angle);
    float y = std::sin(angle);

    circlePositions.push_back(x);
    circlePositions.push_back(y);
    circlePositions.push_back(0.f);

    circleColors.push_back(1.f);
    circleColors.push_back(0.f);
    circleColors.push_back(0.f);
  }

  glGenVertexArrays(1, &vao_circle);
  glBindVertexArray(vao_circle);

  //positions
  glGenBuffers(1, &vbo_circle_position);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_position);
  glBufferData(
    GL_ARRAY_BUFFER,
    circlePositions.size() * sizeof(GLfloat),
    circlePositions.data(),
    GL_STATIC_DRAW
  );

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,            // index du layout (location = 0)
    3,            // nombre de composantes par vertex (x,y,z)
    GL_FLOAT,     // type
    GL_FALSE,     // normalisation
    0,            // stride (0 = données packées)
    (void*)0      // offset
  );

  //couleurs
  glGenBuffers(1, &vbo_circle_colors);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_colors);
  glBufferData(
    GL_ARRAY_BUFFER,
    circleColors.size() * sizeof(GLfloat),
    circleColors.data(),
    GL_STATIC_DRAW
  );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    (void*)0
  );

  glBindVertexArray(0);

  //carré

  // Couleurs pour chacun des 4 sommets (ex: bleu)
  std::vector<GLfloat> squareColors = {
      0.f, 0.f, 1.f,
      0.f, 0.f, 1.f,
      0.f, 0.f, 1.f,
      0.f, 0.f, 1.f
  };

  glGenVertexArrays(1, &vao_surf_bezier); 
  glBindVertexArray(vao_surf_bezier);

  //positions
  glGenBuffers(1, &vbo_bezier_surf_position); // ou vbo_square_position
  glBindBuffer(GL_ARRAY_BUFFER, vbo_bezier_surf_position);
  glBufferData(
    GL_ARRAY_BUFFER,
    squarePositions.size() * sizeof(GLfloat),
    squarePositions.data(),
    GL_STATIC_DRAW
  );

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,         
    3,         
    GL_FLOAT,
    GL_FALSE,
    0,
    (void*)0
  );

  //couleurs
  glGenBuffers(1, &vbo_bezier_surf_colors);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_bezier_surf_colors);
  glBufferData(
    GL_ARRAY_BUFFER,
    squareColors.size() * sizeof(GLfloat),
    squareColors.data(),
    GL_STATIC_DRAW
  );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    (void*)0
  );

  glBindVertexArray(0);
}

void updateSquareVBO() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_bezier_surf_position);

  glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    squarePositions.size() * sizeof(GLfloat),
    squarePositions.data() 
  );
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void recalcCircle()
{
    glm::vec3 c00 = glm::vec3(squarePositions[0], squarePositions[1], squarePositions[2]);
    glm::vec3 c10 = glm::vec3(squarePositions[3], squarePositions[4], squarePositions[5]);
    glm::vec3 c11 = glm::vec3(squarePositions[6], squarePositions[7], squarePositions[8]);
    glm::vec3 c01 = glm::vec3(squarePositions[9], squarePositions[10], squarePositions[11]);

    std::vector<GLfloat> newCirclePositions;
    newCirclePositions.reserve(circleResolution * 3);

    float angleStep = 2.0f * float(M_PI) / circleResolution;
    for (int i = 0; i < circleResolution; ++i)
    {
        float angle = i * angleStep;
        // Coordonnées "initiales" dans [-1,1]^2
        float x = std::cos(angle);
        float y = std::sin(angle);

        // Conversion en [0,1]^2
        float u = (x + 1.f) * 0.5f;
        float v = (y + 1.f) * 0.5f;

        // Interpolation bilinéaire
        glm::vec3 pos =
            c00 * (1.f - u) * (1.f - v)
          + c10 * (      u) * (1.f - v)
          + c11 * (      u) * (      v)
          + c01 * (1.f - u) * (      v);

        // On stocke dans notre tableau
        newCirclePositions.push_back(pos.x);
        newCirclePositions.push_back(pos.y);
        newCirclePositions.push_back(pos.z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_position);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        newCirclePositions.size() * sizeof(GLfloat),
        newCirclePositions.data()
    );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//----------------------------------------
void initOpenGL(GLuint programID)
//----------------------------------------
{
  glCullFace(GL_BACK);    // on spécifie queil faut éliminer les face arriere
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
  glutCreateWindow("EXERCICES DEFORMATION");

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

  traceObjet(); // trace VBO avec ou sans shader

  /* on force l'affichage du resultat */
  glutPostRedisplay();
  glutSwapBuffers();
}

//-------------------------------------
// Trace le tore 2 via le VAO
void traceObjet()
//-------------------------------------
{
  // Use  shader & MVP matrix   MVP = Projection * View * Model;
  glUseProgram(programID);

  // on envoie les données necessaires aux shaders */
  glUniformMatrix4fv(MatrixIDMVP, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(MatrixIDView, 1, GL_FALSE, &View[0][0]);
  glUniformMatrix4fv(MatrixIDModel, 1, GL_FALSE, &Model[0][0]);
  glUniformMatrix4fv(MatrixIDProjection, 1, GL_FALSE, &Projection[0][0]);

  glUniform3f(locCameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);

  glUniform1f(locmaterialShininess, materialShininess);
  glUniform3f(locmaterialSpecularColor, materialSpecularColor.x, materialSpecularColor.y, materialSpecularColor.z);
  glUniform3f(locmaterialAlbedo, materialAlbedo.x, materialAlbedo.y, materialAlbedo.z);
  glUniform3f(locLightPosition, LightPosition.x, LightPosition.y, LightPosition.z);
  glUniform3f(locLightIntensities, LightIntensities.x, LightIntensities.y, LightIntensities.z);
  glUniform1f(locLightAttenuation, LightAttenuation);
  glUniform1f(locLightAmbientCoefficient, LightAmbientCoefficient);

  //cercle
  glBindVertexArray(vao_circle);
  glDrawArrays(GL_LINE_LOOP, 0, circleResolution);
  glBindVertexArray(0);

  //carré
  glBindVertexArray(vao_surf_bezier);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  glBindVertexArray(0);
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
  
  case '1':
    selectedPointIndex = 0;
    break;

  case '2':
    selectedPointIndex = 1;
    break;
  
  case '3':
    selectedPointIndex = 2;
    break;

  case '4':
    selectedPointIndex = 3;
    break;
  
   case 'x':
    squarePositions[3 * selectedPointIndex + 0] += 0.1;
    updateSquareVBO();
    recalcCircle();
    glutPostRedisplay();
    break;

  case 'X':
    squarePositions[3 * selectedPointIndex + 0] -= 0.1;
    updateSquareVBO();
    recalcCircle();
    glutPostRedisplay();
    break;

  case 'y':
    squarePositions[3 * selectedPointIndex + 1] += 0.1;
    updateSquareVBO();
    recalcCircle();
    glutPostRedisplay();
    break;

  case 'Y':
    squarePositions[3 * selectedPointIndex + 1] -= 0.1;
    updateSquareVBO();
    recalcCircle();
    glutPostRedisplay();
    break;

  case 'z':
    squarePositions[3 * selectedPointIndex + 2] += 0.1;
    updateSquareVBO();
    recalcCircle();
    glutPostRedisplay();
    break;

  case 'Z':
    squarePositions[3 * selectedPointIndex + 2] -= 0.1;
    updateSquareVBO();
    recalcCircle();
    glutPostRedisplay();
    break;

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