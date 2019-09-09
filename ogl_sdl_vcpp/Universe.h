#pragma once


#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"
#include "Space.h"

#include <fstream>
#include <iostream>
#include <string>

class Universe
{
public:
    Universe();
    ~Universe();

    int run();
    void onMouseMotion(int dx, int dy);
    void render();
    void initializeGL();
    GLint getAttribLocation(const std::string& attribName);
    GLint getUniformLocation(const std::string& uniformName);
    void initSceneObjects();
    void printGlError();
    void printShaderCompileStatus(GLuint shader);
    void readAndCompileShader(std::string filePath, GLuint &shaderId);
    void compileShaders();
    void linkShaders();
    void useShaderProgram();
    void unuseShaderProgram();

private:
    bool bQuit = false;
    bool bMouseGrabbed = false;
    bool bFastForward = false;
    bool bFastReverse = false;

    bool bLeftMouseButtonDown = false;
    bool bRightMouseButtonDown = false;
    int dx = 0;
    int dy = 0;
    int previousX = 0;
    int previousY = 0;
    float stepMultiplier = 1;
    float previousStepMultiplier = stepMultiplier;


    char F_L_BUTTON_DOWN = 0;
    char F_R_BUTTON_DOWN = 0;
    char F_M_BUTTON_DOWN = 0;

    bool bSidewaysMotionMode = true;
    bool bLockOntoEarth = false;
    bool bLockOntoSun = false;

    /*! \todo F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
        or bLockOntoSun in the function on_MouseMotion().  Need
        to consider if the priority of this check should be reversed.
        Without the setting this flag to 1, the Lock on earth or sun won't
        work. */
    char F_REFERENCE_VECTOR_ALONG_Z = 0;

    bool bExplicitMouseMove = false;


    float angle;

    int curWidth;
    int curHeight;

    GLuint vbo;                 // Create for 
    GLuint axisVao;
    GLuint earthOrbitalPlaneVao;
    GLuint sunVao;
    GLuint earthVao;
    GLuint moonVao;



    GLuint earthLatLongVao;

    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;

    // Texture handles
    GLuint tex1;
    GLuint tex2;

    GLint uniColor;
    GLint uniModel;
    GLint uniView;
    GLint uniProj;

    GLint uniOverrideColor;

    GLint uniMyCenterTransformed;
    //GLint uniMyRadius;
    GLint uniMyIsValud;
    GLint uniMyIsLightSource;

    GLint uniOtherSphereCenterTransformed;
    GLint uniOtherSphereRadius;

    GLint uniSunCenterTransformed;
    GLint uniSunRadius;


    GLint _numAttributes;


    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;


    // Sphere and other objects to be drawn on the screen. Instantiate them here. Their data (vertices) will be created later.
    Axis axis;
    Sphere earth;
    Sphere sun;
    Sphere moon;


    Space space;



};

