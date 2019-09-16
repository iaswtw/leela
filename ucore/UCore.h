#pragma once


#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"
#include "Space.h"
#include "OglHandles.h"

#include <fstream>
#include <iostream>
#include <string>

#include "OneShotBoolean.h"

#define MAXSTARS 1800
#define MAXGALAXYSTARS 10000



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
    USimulationSpeed_Low2,
    USimulationSpeed_Low1,
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
    UDemo_TotalSolarEclipseOnNorthPole,
    UDemo_AnnularSolarEclipseFromSpace,
    UDemo_PartialLunarEclipse,
    UDemo_TiltedOrbitalPlanes,
    UDemo_PrecessionMotion
} UDemoType;



class Universe
{
public:
    Universe();
    ~Universe();

    void generateStars();
    void initSceneObjects();
    void processFlags();


    void advance(float stepMultiplier);


public:

    int dx = 0;
    int dy = 0;
    float _stepMultiplier = 1;
    
    bool bStars = true;
    
    /*! \todo F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
        or bLockOntoSun in the function on_MouseMotion().  Need
        to consider if the priority of this check should be reversed.
        Without the setting this flag to 1, the Lock on earth or sun won't
        work. */
    char F_REFERENCE_VECTOR_ALONG_Z = 0;

    bool bSimulationPause = false;

    USimulationSpeedType eSimulationSpeed;
    UTimeDirectionType eTimeDirection;
    UDotDensityType eDotDensity;


    float angle = 0.0f;

    
    // Texture handles
    GLuint tex1;
    GLuint tex2;

    Axis axis;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;


    // Sphere and other objects to be drawn on the screen. Instantiate them here. Their data (vertices) will be created later.
    Sphere earth;
    Sphere sun;
    Sphere moon;

    Space space;

    PNT star[MAXSTARS];
    PNT gstar[MAXGALAXYSTARS];

    std::vector<float> starVertices;
    std::vector<float> gstarVertices;


    bool bUpdateUI;

};

