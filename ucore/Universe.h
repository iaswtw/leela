#pragma once


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"
#include "SphereRenderer.h"
#include "AxisRenderer.h"
#include "Stars.h"
#include "StarsRenderer.h"
#include "Space.h"
#include <fstream>
#include <iostream>
#include <string>

#include "imgui.h"
#include "OneShotBoolean.h"
#include "GlslProgram.h"

constexpr auto MAXGALAXYSTARS = 10000;



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
    USimulationSpeed_1p5625_Percent = 0,
    USimulationSpeed_3p125_Percent,
    USimulationSpeed_6p25_Percent,
    USimulationSpeed_12p5_Percent,
    USimulationSpeed_25_Percent,
    USimulationSpeed_50_Percent,
    USimulationSpeed_100_Percent,
    USimulationSpeed_400_Percent,
    USimulationSpeed_1600_Percent
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
    UDemo_TotalSolarEclipseViewFromSun,
    UDemo_TotalSolarEclipseOnNorthPole,
    UDemo_AnnularSolarEclipseFromSpace,
    UDemo_PartialLunarEclipse,
    UDemo_TiltedOrbitalPlanes,
    UDemo_SixMonthLongDayAndNightOnNorthPole,
    UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle,
    UDemo_PrecessionMotion,
    UDemo_StarParallex
} UDemoType;


typedef enum
{
    TargetLockMode_ViewTarget,        // camera position remains fixed, but always looks at the target
    TargetLockMode_FollowTarget       // camera moves with the object maintaining distance and direction
} TargetLockMode;

class Universe
{
public:
    Universe();    
    ~Universe();

    void HelpMarker(const char* desc);

    int run();
    int runMainLoop();

    void processFlags();
    void navigate(float __throttle, float __yaw, float __pitch, float __roll);
    void render();
    void renderAllNontransparentObjects();
    void renderAllTransparentObjects();
    void renderUsingPlanetGlslProgram();
	void renderUsingStarGlslProgram();
	void renderUsingSunGlslProgram();
    void renderUsingSimpleGlslProgram();
    void renderTransparentUsingSimpleGlslProgram();



    void initializeGL();
    void initSceneObjects();
    void printGlError();

    void ChangeSidewaysMotionMode();
    void SetDefaultView();
    void SetApplicationStartView();
    
    void SetLockTargetAndMode(Sphere* target, TargetLockMode mode);
    void SetLockMode(TargetLockMode mode);
    void ToggleConstantDirectionFollowMode();
    void ResetFollowTargetAndMode();
    void ToggleFollowTarget(Sphere* target, TargetLockMode mode);

    void LookAtTarget();

    void IncreaseSimulationSpeed();
    void DecreaseSimulationSpeed();
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
    void Earth_Orbit(int nParam);

    void Moon_OrbitalPlaneRotation(int nParam);
    void Moon_Orbit(int nParam);
    void Moon_RevolutionMotion(int nParam);
    void Moon_SetOrbitalPositionAngle(double fAngle);

    void SetDotDensity(int nParam);
    void ShowDemo(int nParam);

    void ChangeBoolean(bool *pBool, int nParam);

    void advance(float stepMultiplier);

    void onKeyDown(SDL_Event* event);
    void onKeyUp(SDL_Event* event);
    void onMouseMotion(int xrel, int yrel);

    void toggleFullScreen();
    void setWidgetControlMode();
    void resetWidgetControlMode();
    void cleanupAndExitApplication();
    void generateImGuiWidgets();

private:
    bool bQuit = false;
    bool bMouseGrabbed = false;
    bool bFastForward = false;
    bool bFastReverse = false;

    int dx = 0;
    int dy = 0;
    int previousX = 0;
    int previousY = 0;
    float _stepMultiplier = 1.0f;
    float _stepMultiplierFrameRateAdjustment = 0.0f;


    bool bLeftMouseButtonDown = false;
    bool bRightMouseButtonDown = false;
    bool bMiddleMouseButtonDown = false;

    bool bMoveForward = false;
    bool bMoveBackward = false;
    bool bRotateRight = false;
    bool bRotateLeft = false;
    bool bRotateRightSd = false;
    bool bRotateLeftSd = false;


    const float noYaw               = 0.0f;
    const float noPitch             = 0.0f;
    const float noRoll              = 0.0f;
    const float noThrottle          = 0.0f;

    const float nominalYaw          = 0.5f;
    const float nominalPitch        = 0.5f;
    const float nominalRoll         = 0.5f;
    const float nominalThrottle     = 30.0f;


    float yaw       = 0.0f;
    float pitch     = 0.0f;
    float roll      = 0.0f;
    float throttle  = 0.0f;


    bool bCtrlModifier = false;
    bool bAltModifier = false;
    bool bShiftModifier = false;

    bool bSidewaysMotionMode = true;

    bool bGalaxyStars = false;
    bool bShowAxis = true;
    bool bShowOrbitsGlobalEnable = true;           // Individual orbit enables are in respective renderer classes.

    TargetLockMode lockMode = TargetLockMode_ViewTarget;
    VECTOR followVector = VECTOR(1.0, 1.0, 1.0);
    float followDistance = 0.0f;
    Sphere* lockTarget = nullptr;           // If not null, the camera will change direction such that target
                                            // will always appear at the center of the screen.


    bool bSimulationPause = false;

    USimulationSpeedType eSimulationSpeed;
    UTimeDirectionType eTimeDirection;
    UDotDensityType eDotDensity;


    float angle = 0.0f;

    int curWidth = 0;
    int curHeight = 0;


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
    Stars stars;

    AxisRenderer axisRenderer;
    PlanetRenderer earthRenderer;
    PlanetRenderer moonRenderer;
    SunRenderer sunRenderer;
    StarsRenderer starsRenderer;
    
    Space space;

    PNT gstar[MAXGALAXYSTARS];

    std::vector<float> starVertices;
    std::vector<float> twoPixelWideStarVertices;
    std::vector<float> gstarVertices;

	GlslProgram planetGlslProgram;
	GlslProgram sunGlslProgram;
	GlslProgram starGlslProgram;
    GlslProgram simpleGlslProgram;

    // Realistic day/night shading, shadow shading.
    // Effect on day & nights:
    //   When false, day & night hemispheres will he equal.
    //   When true, night hemisphere will be smaller than depending on the size of the sun.
    bool bRealisticShading = true;
    bool bRealisticSurfaces = true;

    //=================================================================
    // IMGUI, SDL related
    SDL_Window *window = nullptr;
    SDL_GLContext context;

    bool bAlwaysShowControlPanel = true;
    bool bIsWindowFullScreen = false;
    bool bShowFlagsOverlay = true;
    bool bShowMouseNavigationHelp = false;
    bool bShowKeyboardShortcuts = false;
    bool bShowIntroduction = false;

    ImFont *appFontExtraSmall = nullptr;
    ImFont *appFontSmall = nullptr;
    ImFont *appFontSmallMedium = nullptr;
    ImFont *appFontMedium = nullptr;
    ImFont *appFontLarge = nullptr;
    ImFont *appFontGiant = nullptr;
    ImFont *fixedWidthSmall = nullptr;

    OneShotBoolean doubleClicked = OneShotBoolean();

    std::string logString = "";

};

