#pragma once


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "SceneObject.h"
#include "SphericalBody.h"
#include "SphericalBodyRenderer.h"
#include "CoordinateAxisRenderer.h"
#include "BookmarkRenderer.h"
#include "MonthLabelsRenderer.h"
#include "LatLonRenderer.h"
#include "Stars.h"
#include "StarsRenderer.h"
#include "Space.h"
#include "Fir.h"
#include <fstream>
#include <iostream>
#include <string>
#include "map"
#include <stack>
#include "UniverseMinimal.h"
#include "ViewportSceneObject.h"

#include "imgui.h"
#include "OneShotTimer.h"
#include "GlslProgram.h"
#include <ft2build.h>
#include FT_FREETYPE_H


#include <spdlog/spdlog.h>


constexpr auto MAXGALAXYSTARS = 10000;
constexpr auto NUM_NAVIGATION_INPUT_SAMPLES = 10;
constexpr auto FIR_WIDTH = 100;


#define RELEASE_BUILD
//#define USE_ICOSPHERE

struct Character {
    unsigned int textureID;     // ID handle of the glyph texture
    glm::ivec2 size;            // size of glyph
    glm::ivec2 bearing;         // offset from baseline to top/left of glyph
    unsigned int advance;       // offset to advance to next glyph
};


class Action
{
public:
    Action(std::function<void()> work_func, std::string help)
        : 
        _work_func(work_func),
        _help(help)
    {}

    ~Action() {}

    void invoke()
    {
        if (_work_func) {
            _work_func();
        }
    }


private:
    std::function<void()> _work_func;
    std::string _help;
};


// map between SDK key info and a vector of functions.
typedef std::map<std::tuple<int, int>, Action> ActionMap;



/*!
 **************************************************************************

 In response to the interaction of the user with the GUI widgets, the GUI
 should send these commands to the Leela core

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

typedef enum
{
    UTimeDirection_Forward,
    UTimeDirection_Reverse
} UTimeDirectionType;

typedef enum
{
    UDotDensity_Normal,
    UDotDensity_High
} UDotDensityType;

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
    UDemo_StarParallex,
    UDemo_MoonsNodalPrecession,
    UDemo_ApparentRetrogradeMotion,
    UDemo_MercuryTransit,
} UDemoType;


typedef enum
{
    TargetLockMode_OrientedViewTarget,  // camera moves with the object; maintains orientation w.r.t parent of sphere.
                                        //    e.g. if lock target was earth, and sun was to the left, sun will remain to the left as the earth
                                        //         and the camera move in the orbit.
    TargetLockMode_ViewTarget,          // camera position remains fixed, but always looks at the target
    TargetLockMode_FollowTarget         // camera moves with the object maintaining distance and direction
} TargetLockMode;


typedef enum
{
    RenderTextType_ObjectText,      // text is like a 3d object such as sphere and is to be placed at x,y,z in the scene.
                                    //  - text size changes based on the distance and orientation of text w.r.t the observer
    RenderTextType_ScreenText       // text is to be placed closest to the user at the beginning of the frustm
                                    //  - text appears the same size regardless of observer's position
} RenderTextType;




class Leela;
class Space;

//-----------------------------------------
// Globally available handles
//-----------------------------------------
extern Leela* g_leela;
extern Space* g_space;



class Leela
{
public:
    Leela();    
    ~Leela();

    void createActions();

    void HelpMarker(const char* desc);

    int run();
    int runMainLoop();

    void processFlags();
    void navigate(float __throttle, float __yaw, float __pitch, float __roll);
    void render();
    void renderAllViewportTypes();
    void renderAllStages(ViewportType viewportType);
    void renderUsingAllShaderPrograms(ViewportType viewportType, RenderStage renderStage);
    bool setupViewport(ViewportType viewportType);
    void renderSceneUsingGlslProgram(RenderStage renderStage, GlslProgram& glslProgram, ViewportType viewportType);
    void renderSceneObjectUsingGlslProgram(SceneObject* sceneObject, RenderStage renderStage, GlslProgram& glslProgram, ViewportType viewportType);
    void RenderText(GlslProgram& glslProgram, RenderTextType renderType, std::string text, float x, float y, float z, float scale, glm::vec3 color);

    void constructFontInfrastructureAndSendToGpu();

    void compileShaders();
    void initSceneObjectsAndComponents();
    void printGlError();

    void ChangeSidewaysMotionMode();
    void SetDefaultView();
    void SetApplicationStartView();
    
    void SetLockTargetAndMode(SphericalBody* target, TargetLockMode mode);
    void SetLockMode(TargetLockMode mode);
    void calculateCommonTargetLockVariables();
    void calculateFollowTargetLockVariables();
    void calculateOrientedViewLockVariables();
    void cycleLockMode();
    void cycleLabelModes();
    void hideMonthLabels();
    void showLargeMonthLabels();
    void showNormalMonthLabels();
    void ResetFollowTargetAndMode();
    void ToggleFollowTarget(SphericalBody* target, TargetLockMode mode);

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
    void Moon_ResetNodalPrecession();
    bool IsAnyOrbitVisible();
    bool _IsAnyOrbitVisible(SceneObject* sceneObject);
    void DisableShowingAllOrbits();
    void _DisableShowingOrbit(SceneObject* scene);
    void ShowAllOrbits();
    void _ShowAllOrbits(SceneObject* sceneObject, vector<string>& names);
    void ShowEarthAndMoonOrbits();
    void Moon_RevolutionMotion(int nParam);
    void Moon_SetOrbitalPositionAngle(double fAngle);

    void SetDotDensity(int nParam);
    void ShowDemo(int nParam);

    void ChangeBoolean(bool *pBool, int nParam);

    void _advanceSceneObject(SceneObject * sceneObject, float stepMultiplier);
    void advanceScene(float stepMultiplier);

    void onKeyDown(SDL_Event* event);
    void onKeyUp(SDL_Event* event);
    void onMouseMotion(int xrel, int yrel);

    void toggleFullScreen();
    void toggleControlPanelVisibilityWhenMouseGrabbed();
    void setWidgetControlMode();
    void resetWidgetControlMode();
    void cleanupAndExitApplication();
    bool SmallCheckbox(const char* label, bool* v);
    void generateImGuiWidgets();

    bool bShowLowDarknessAtNight = false;
    bool bShowWireframeSurfaces = false;

    void clearAllFirFilters();

    int getHeightOfCharA();
    void createFontCharacterTexture();

    glm::vec3 getScreenCoordinates(glm::vec3 scenePoint);
    float findScreenProjectedHeight(float segmentLength, glm::vec3 atPoint);

    bool isNoModifier();
    bool isAllModifiers();

public:
    bool bQuit = false;
    bool bMouseGrabbed = false;
    bool bFastForward = false;
    bool bFastReverse = false;

    bool bAdvanceEarthInOrbit = false;
    bool bRetardEarthInOrbit = false;
    bool bAdvanceMoonInOrbit = false;
    bool bRetardMoonInOrbit = false;

    int previousX = 0;
    int previousY = 0;
    float _stepMultiplier = 1.0f;
    float _filteredStepMultiplier = 0.0f;
    float _stepMultiplierFrameRateAdjustment = 0.0f;

    bool bMinus = false;
    bool bEquals = false;

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

    //
    // use the following python to create fir coefficients
    //      set value of FIR_WIDTH
    //      dx = 2*math.pi / FIR_WIDTH
    //      x = [dx*i for i in range(1, FIR_WIDTH + 1)]
    //      y = [(1-math.cos(i))/2 for i in x]
    //
    float fir_coeff[FIR_WIDTH] = { 0.006155829702431115f, 0.024471741852423234f, 0.05449673790581605f, 0.09549150281252627f, 0.1464466094067262f, 0.20610737385376343f, 0.2730047501302266f, 0.3454915028125263f, 0.4217827674798845f, 0.49999999999999994f, 0.5782172325201154f, 0.6545084971874737f, 0.7269952498697734f, 0.7938926261462365f, 0.8535533905932737f, 0.9045084971874737f, 0.9455032620941839f, 0.9755282581475768f, 0.9938441702975689f, 1.0f, 1.0f, 0.9394130628134758f, 0.8824969025845955f, 0.8290291181804004f, 0.7788007830714049f, 0.7316156289466418f, 0.6872892787909722f, 0.645648526427892f, 0.6065306597126334f, 0.569782824730923f, 0.5352614285189903f, 0.5028315779709409f, 0.4723665527410147f, 0.44374731008107987f, 0.4168620196785084f, 0.391605626676799f, 0.36787944117144233f, 0.3455907525769745f, 0.32465246735834974f, 0.3049827687110593f, 0.2865047968601901f, 0.26914634872918386f, 0.25283959580474646f, 0.23752081909545814f, 0.22313016014842982f, 0.2096113871510978f, 0.19691167520419406f, 0.18498139990730428f, 0.17377394345044514f, 0.1632455124539584f, 0.15335496684492847f, 0.14406365910145327f, 0.1353352832366127f, 0.1271357329320356f, 0.11943296826671962f, 0.11219689052034373f, 0.10539922456186433f, 0.0990134083638263f, 0.09301448921066349f, 0.08737902619542039f, 0.0820849986238988f, 0.07711171996831671f, 0.07243975703425146f, 0.0680508540250102f, 0.06392786120670757f, 0.060054667895307945f, 0.05641613950377735f, 0.0529980584033558f, 0.049787068367863944f, 0.04677062238395898f, 0.04393693362340742f, 0.04127492938579755f, 0.03877420783172201f, 0.036424997337364234f, 0.03421811831166603f, 0.03214494732687607f, 0.0301973834223185f, 0.0283678164497131f, 0.026649097336355485f, 0.025034510149960148f, 0.023517745856009107f, 0.022092877665062443f, 0.020754337873699742f, 0.019496896108597995f, 0.01831563888873418f, 0.017205950425851383f, 0.016163494588165874f, 0.015184197956837946f, 0.014264233908999256f, 0.013400007665140828f, 0.012588142242433998f, 0.011825465259096618f, 0.011108996538242306f, 0.010435936462774504f, 0.009803655035821828f, 0.00920968160396814f, 0.008651695203120634f, 0.008127515489292211f, 0.007635094218859962f, 0.007172507245008699f };

    float mouse_throttle = 0.0f;
    float mouse_yaw = 0.0f;
    float mouse_pitch = 0.0f;
    float mouse_roll = 0.0f;

    FirFilter throttleFilter;
    FirFilter yawFilter; 
    FirFilter pitchFilter;
    FirFilter rollFilter;
    FirFilter stepMultiplierFilterWhenPaused;
    FirFilter stepMultiplierFilter;

    bool bCtrlModifier = false;
    bool bAltModifier = false;
    bool bShiftModifier = false;

    bool bSidewaysMotionMode = true;

    bool bGalaxyStars = false;
    bool bShowAxis = true;
    bool bShowPlanetAxis = false;
    bool bShowOrbitsGlobalEnable = true;           // Individual orbit enables are in respective renderer classes.
    bool bShowMonthNames = true;
    bool bMonthLabelsCloserToSphere = false;
    bool bShowLabelsOnTop = false;
    bool bShowLargeLabels = false;

    TargetLockMode lockMode = TargetLockMode_ViewTarget;
    VECTOR followVector = VECTOR(1.0, 1.0, 1.0);
    float followDistance = 0.0f;
    float orientedTargetLock_alpha = 0.0f;
    float orientedTargetLock_beta = 0.0f;
    VECTOR downDirection = VECTOR(0.0f, 0.0f, 0.0f);

    SphericalBody* lockTarget = nullptr;           // If not null, the camera will change direction such that target
                                            // will always appear at the center of the screen.
    bool bEarthSurfaceLockMode = false;
    float surfaceLockTheta = 1.0f;      // change which latitude camera is at in degrees.
    float surfaceLockAlpha = 0.0f;      // don't intend to use this. Don't see enough value.

    bool bSimulationPause = false;

    USimulationSpeedType eSimulationSpeed;
    UTimeDirectionType eTimeDirection;
    UDotDensityType eDotDensity;


    float angle = 0.0f;

    ViewportSceneObject* minimapViewport = nullptr;
    ViewportSceneObject* alternateObserverViewport = nullptr;


    int curX = 0;
    int curY = 0;
    int curWidth = 0;
    int curHeight = 0;

    int curViewportX = 0;
    int curViewportY = 0;
    int curViewportWidth = 0;
    int curViewportHeight = 0;

    // Texture handles
    GLuint tex1 = 0;
    GLuint tex2 = 0;

    GLuint fontVao, largeFontVao = 0;
    GLuint fontVbo, largeFontVbo = 0;

    FT_Library ft;
    FT_Face face;

    std::map<char, Character> characters = {};
    std::map<char, Character> largeCharacters = {};


    GLint uniOverrideColor;

    glm::mat4 viewMatrix        = glm::mat4(1.0f);
    glm::mat4 projectionMatrix  = glm::mat4(1.0f);


    // SphericalBody and other objects to be drawn on the screen. Instantiate them here. Their data (vertices) will be created later.
    CoordinateAxisRenderer coordinateAxisRenderer;
    SphericalBody* mercury = nullptr;
    SphericalBody* earth = nullptr;
    SphericalBody* sun = nullptr;
    SphericalBody* mars = nullptr;
    SphericalBody* jupiter = nullptr;
    SphericalBody* uranus = nullptr;
    SphericalBody* moon = nullptr;
    Stars stars;

    Scene scene;

    SunRenderer *sunRenderer = nullptr;
    PlanetRenderer *earthRenderer = nullptr;
    PlanetRenderer *moonRenderer = nullptr;
    PlanetRenderer *marsRenderer = nullptr;

    LatLonRenderer* earthLatLonRenderer = nullptr;

    StarsRenderer starsRenderer;
    MonthLabelsRenderer* monthLabelsRenderer;        // for earth

    Space space;

    PNT gstar[MAXGALAXYSTARS];

    std::vector<float> starVertices;
    std::vector<float> twoPixelWideStarVertices;
    std::vector<float> gstarVertices;

    std::vector<GlslProgram*> shaderPrograms;

    // Realistic day/night shading, shadow shading.
    // Effect on day & nights:
    //   When false, day & night hemispheres will he equal.
    //   When true, night hemisphere will be smaller than depending on the size of the sun.
    bool bRealisticShading = true;
    bool bRealisticSurfaces = true;

    ActionMap actionMap;

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

    OneShotTimer doubleClicked = OneShotTimer();

    std::string minimapMode = "Zoomed Out";
    std::vector<std::string> minimapModes = { "Zoomed Out", "Rear View" };

    std::string nightDarknessLevelStr = "High";
    std::vector<std::string> nightDarknessLevelStrs = { "Black", "Very High", "High", "Medium", "Low", "Very Low", "None"};

    std::string logString = "";


};

