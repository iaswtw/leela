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
constexpr auto NUM_NAVIGATION_INPUT_SAMPLES = 10;
constexpr auto FIR_WIDTH = 100;


#define RELEASE_BUILD

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
    float applyFir(float* firFilterCoefficients, float* inputVector, unsigned filterWidth);
    void shiftValues(float* values, unsigned int numValues);
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

    const float REFERENCE_FRAME_RATE = 60.0f;


    const float noYaw               = 0.0f;
    const float noPitch             = 0.0f;
    const float noRoll              = 0.0f;
    const float noThrottle          = 0.0f;

    const float nominalYaw          = 0.5f;
    const float nominalPitch        = 0.5f;
    const float nominalRoll         = 0.5f;
    const float nominalThrottle     = 30.0f;




    //----------------------------------------------
    // Keyboard
    //----------------------------------------------
    float keyboard_yaw       = 0.0f;
    float keyboard_pitch     = 0.0f;
    float keyboard_roll      = 0.0f;
    float keyboard_throttle  = 0.0f;

    //----------------------------------------------
    // Mouse inputs.  Just arrived mouse inputs are processed in the next iteration and then cleared.
    //----------------------------------------------
    float mouse_dx = 0.0f;
    float mouse_dy = 0.0f;
    float new_mouse_wheel_throttle = 0.0f;


    // low pass filtered inputs in various directions.
    float throttle    = 0.0f;
    float yaw         = 0.0f;
    float pitch       = 0.0f;
    float roll        = 0.0f;

    //
    // use the following python to create fir coefficients
    //      set value of FIR_WIDTH
    //      dx = 2*math.pi / FIR_WIDTH
    //      x = [dx*i for i in range(1, FIR_WIDTH + 1)]
    //      y = [(1-math.cos(i))/2 for i in x]
    //
    float fir_coeff[FIR_WIDTH] = { 0.006155829702431115, 0.024471741852423234, 0.05449673790581605, 0.09549150281252627, 0.1464466094067262, 0.20610737385376343, 0.2730047501302266, 0.3454915028125263, 0.4217827674798845, 0.49999999999999994, 0.5782172325201154, 0.6545084971874737, 0.7269952498697734, 0.7938926261462365, 0.8535533905932737, 0.9045084971874737, 0.9455032620941839, 0.9755282581475768, 0.9938441702975689, 1.0, 1.0, 0.9394130628134758, 0.8824969025845955, 0.8290291181804004, 0.7788007830714049, 0.7316156289466418, 0.6872892787909722, 0.645648526427892, 0.6065306597126334, 0.569782824730923, 0.5352614285189903, 0.5028315779709409, 0.4723665527410147, 0.44374731008107987, 0.4168620196785084, 0.391605626676799, 0.36787944117144233, 0.3455907525769745, 0.32465246735834974, 0.3049827687110593, 0.2865047968601901, 0.26914634872918386, 0.25283959580474646, 0.23752081909545814, 0.22313016014842982, 0.2096113871510978, 0.19691167520419406, 0.18498139990730428, 0.17377394345044514, 0.1632455124539584, 0.15335496684492847, 0.14406365910145327, 0.1353352832366127, 0.1271357329320356, 0.11943296826671962, 0.11219689052034373, 0.10539922456186433, 0.0990134083638263, 0.09301448921066349, 0.08737902619542039, 0.0820849986238988, 0.07711171996831671, 0.07243975703425146, 0.0680508540250102, 0.06392786120670757, 0.060054667895307945, 0.05641613950377735, 0.0529980584033558, 0.049787068367863944, 0.04677062238395898, 0.04393693362340742, 0.04127492938579755, 0.03877420783172201, 0.036424997337364234, 0.03421811831166603, 0.03214494732687607, 0.0301973834223185, 0.0283678164497131, 0.026649097336355485, 0.025034510149960148, 0.023517745856009107, 0.022092877665062443, 0.020754337873699742, 0.019496896108597995, 0.01831563888873418, 0.017205950425851383, 0.016163494588165874, 0.015184197956837946, 0.014264233908999256, 0.013400007665140828, 0.012588142242433998, 0.011825465259096618, 0.011108996538242306, 0.010435936462774504, 0.009803655035821828, 0.00920968160396814, 0.008651695203120634, 0.008127515489292211, 0.007635094218859962, 0.007172507245008699 };

    float mouse_throttle = 0.0f;
    float mouse_yaw = 0.0f;
    float mouse_pitch = 0.0f;
    float mouse_roll = 0.0f;

    float new_throttle[FIR_WIDTH] = { 0.0f };
    float new_yaw[FIR_WIDTH] = { 0.0f };
    float new_pitch[FIR_WIDTH] = { 0.0f };
    float new_roll[FIR_WIDTH] = { 0.0f };


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
    bool bEarthSurfaceLockMode = false;
    float surfaceLockTheta = 1.0f;      // change which latitude camera is at.
    float surfaceLockAlpha = 0.0f;      // don't intend to use this. Don't see enough value.

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
    Sphere mars;
    Sphere moon;
    Stars stars;

    AxisRenderer axisRenderer;
    PlanetRenderer earthRenderer;
    PlanetRenderer moonRenderer;
    PlanetRenderer marsRenderer;
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

