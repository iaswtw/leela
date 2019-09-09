#pragma once


#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"
#include "SphereRenderer.h"
#include "Space.h"
#include "OglHandles.h"

#include <fstream>
#include <iostream>
#include <string>



/*!
 **************************************************************************

 In response to the interaction of the user with the GUI widgets, the GUI
 should send these commands to the Universe core

 **************************************************************************/
typedef enum
{
    UCmd_None,
    UCmd_ChangeSidewaysMotionMode,
    UCmd_ChangeAxis,
    UCmd_ChangeOrbit,
    UCmd_EarthsOrbitalPlane,
    UCmd_MoonsOrbitalPlane,
    UCmd_DefaultView,

    UCmd_FastForward,
    UCmd_Rewind,

    UCmd_MoonsOrbitalPlaneRotation,
    UCmd_MoonsRevolutionMotion,

    UCmd_LockOntoSun,
    UCmd_LockOntoEarth,

    /* Space Movements */
    UCmd_ZoomIn,
    UCmd_ZoomOut,
    UCmd_MoveLeft,
    UCmd_MoveRight,
    UCmd_MoveDown,
    UCmd_MoveUp,
    UCmd_FasterZoomIn,
    UCmd_FasterZoomOut,
    UCmd_FastestZoomIn,
    UCmd_FastestZoomOut,


    UCmd_ChangeStarDistribution,
    UCmd_PositionEarthAt0Y0,
    UCmd_PositionEarthAtMinusX00,
    UCmd_PositionEarthAt0MinusY0,
    UCmd_PositionEarthAtX00,

    UCmd_Pause,

    /* Earth's Motions */
    UCmd_EarthsRotationMotion,
    UCmd_EarthsRevolutionMotion,
    UCmd_EarthsPrecessionMotion, /*! Operate on Earth's Precession - Start, Stop, Reset */

    UCmd_SetSimulationSpeed,
    UCmd_IncreaseSimulationSpeed,
    UCmd_DecreaseSimulationSpeed,

    UCmd_SetTimeDirection,
    UCmd_SetDotDensity,

    UCmd_ShowDemo,

    UCmd_Invalid,

} UCommandType;

/*!
 **************************************************************************
  Generic Parameter types used for various commands with similar 'verbs'

 **************************************************************************/
typedef enum
{
    UCmdParam_Reset,
    UCmdParam_Toggle,

    UCmdParam_Start,
    UCmdParam_Stop,

    UCmdParam_On,
    UCmdParam_Off,
} UCmdParamType;

/*!
 **************************************************************************

 **************************************************************************/
typedef enum
{
    USimulationSpeed_VeryLow = 0,
    USimulationSpeed_Low,
    USimulationSpeed_Normal,
    USimulationSpeed_High,
    USimulationSpeed_VeryHigh
} USimulationSpeedType;


/*!
 **************************************************************************

 **************************************************************************/
typedef enum
{
    UTimeDirection_Forward,
    UTimeDirection_Reverse
} UTimeDirectionType;


/*!
 **************************************************************************

 **************************************************************************/
typedef enum
{
    UDotDensity_Normal,
    UDotDensity_High
} UDotDensityType;

/*!
 **************************************************************************

 **************************************************************************/
typedef enum
{
    UDemo_TotalSolarEclipse,
    UDemo_TiltedOrbitalPlanes,
    UDemo_PrecessionMotion
} UDemoType;



class Universe
{
public:
    Universe();
    ~Universe();

    int run();

    void render();
    void initializeGL();
    void initSceneObjects();
    void printGlError();
    void printShaderCompileStatus(GLuint shader);
    void readAndCompileShader(std::string filePath, GLuint &shaderId);
    void compileShaders();
    void linkShaders();
    void useShaderProgram();
    void unuseShaderProgram();

    void ChangeSidewaysMotionMode();
    void SetDefaultView();
    void NavigationLockOntoEarth(int nParam);
    void NavigationLockOntoSun(int nParam);
    void SetSimulationSpeed(int nParam);
    void SetTimeDirection(int nParam);
    void FastForward(int nParam);
    void Rewind(int nParam);
    void SimulationPause(int nParam);
    void Earth_RotationMotion(int nParam);
    void Earth_RevolutionMotion(int nParam);
    void Earth_PrecessionMotion(int nParam);

    void Earth_OrbitalPlane(int nParam);
    void Earth_SetOrbitalPositionAngle(double fAngle);
    void Moon_OrbitalPlane(int nParam);
    void Moon_RevolutionMotion(int nParam);
    void Moon_SetOrbitalPositionAngle(double fAngle);



    void ChangeBoolean(bool *pBool, int nParam);

    void advance(float stepMultiplier);

    void onKeyDown(SDL_Event* event);
    void onKeyUp(SDL_Event* event);
    void onMouseMotion(SDL_Event* event);

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
    float _stepMultiplier = 1;


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


    bool bSimulationPause;

    USimulationSpeedType eSimulationSpeed;
    UTimeDirectionType eTimeDirection;
    UDotDensityType eDotDensity;


    float angle;

    int curWidth;
    int curHeight;


    OglHandles oglHandles;

    GLuint vbo;                 // Create for 
    GLuint earthOrbitalPlaneVao;
    GLuint sunVao;
    GLuint earthVao;
    GLuint moonVao;
       
    GLuint earthLatLongVao;
    
    // Texture handles
    GLuint tex1;
    GLuint tex2;

    GLint uniOverrideColor;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;


    // Sphere and other objects to be drawn on the screen. Instantiate them here. Their data (vertices) will be created later.
    Axis axis;
    Sphere earth;
    Sphere sun;
    Sphere moon;

    SphereRenderer earthRenderer;
    SphereRenderer sunRenderer;
    SphereRenderer moonRenderer;

    Space space;

    bool bUpdateUI;
};

