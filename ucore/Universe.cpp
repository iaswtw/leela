#include "pch.h"

#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include "Universe.h"
#include "Utils.h"
#include <stdio.h>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>


// Following function was copied from imgui_demo.cpp
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
void Universe::HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1));
        ImGui::BeginTooltip();
        ImGui::PushFont(fixedWidthSmall);

        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        
        ImGui::PopFont();
        ImGui::EndTooltip();
        ImGui::PopStyleColor();
    }
}

static std::string FindFontFile(const char * fileName)
{
    struct stat fileStat;
    std::string fullPath;
    
    fullPath = std::string("fonts/") + fileName;
    if (stat(fullPath.c_str(), &fileStat) == 0)
        return std::string(fullPath);

    fullPath = std::string("../external/fonts/") + fileName;
    if (stat(fullPath.c_str(), &fileStat) == 0)
        return std::string(fullPath);

    // File not found
    return std::string("");
}

Universe::Universe() :
    sunRenderer(sun, "../ucore/textures/sunmap.png"),
    //earthRenderer(earth, "../ucore/textures/Map-World_23_10.png"),
    //earthRenderer(earth, "../ucore/textures/earthmap1k.png"),
    earthRenderer(earth, "../ucore/textures/earth-erde-mit-wolken-2k.png"),
    
    //earthRenderer(earth, "../ucore/textures/cat1.png"),
    //moonRenderer(moon),
    moonRenderer(moon, "../ucore/textures/moonmap1k.png"),
    axisRenderer(axis),
    starsRenderer(stars)
{
}


Universe::~Universe()
{
}

/*************************************************************************************************
 Initialize various parameters of planets, stars, etc.  Parameters include radius of objects,
 time periods of rotation and revolution, colors, etc.
**************************************************************************************************/
void Universe::initSceneObjects()
{
    stars.setCubeStarParameters(
        3500,                               // total number of stars (single + double pixel)
        0.6f                                // radio of single pixel starts to all stars
    );


    axis.setSpan(3600, 3600, 1200);
    axis.setColors(
        glm::vec3(0.2f, 0.2f, 0.5f),        // X axis color
        glm::vec3(0.2f, 0.5f, 0.2f),        // Y axis color
        glm::vec3(0.2f, 0.5f, 0.5f)         // Z axis color
    );
    // Sun
    //---------------------------------------
    //sun.setColor(0.7f, 0.7f, 0.1f);
    sun.setColor(1.0f, 1.0f, 0.6f);
    sun.setRotationParameters(160,          // radius
        0,                                  // initial rotation angle
        0.002f,                              // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(0.0f)                  // axis tilt angle
    );
    sun.setOrbitalParameters(0,             // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                              // revolution velocity
        0,                                  // orbital rotation angle
        0                                   // orbital tilt
    );


    // Earth
    //---------------------------------------
    earth.setColor(0.55f, 0.82f, 1.0f);
    earth.setRotationParameters(80,         // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(270.0f),               // axis rotation angle
        glm::radians(23.5f)                 // axis tilt angle
    );
    earth.setOrbitalParameters(3000,        // radius of orbit
        glm::radians(0.0f),                 // initial orbital angle
        0.001f,                             // revolution velocity
        0.0f,                               // orbital rotation angle
        0                                   // orbital tilt
    );
    earth.setOrbitalPlaneColor(glm::vec3(0.55, 0.82, 1.0));

    // Moon
    //---------------------------------------
    moon.setColor(0.8f, 0.8f, 0.8f);
    moon.setRotationParameters(22,          // radius
        0,                                  // initial rotation angle
        0.008888888f,                       // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(10.0f)                 // axis tilt angle
    );
    moon.setOrbitalParameters(400,          // radius of orbit
        0.0f,                               // initial orbital angle
        0.008888888f,                       // revolution velocity
        0,                                  // orbital rotation angle
        glm::radians(30.0f)                 // orbital tilt
    );
    moon.setOrbitalPlaneColor(glm::vec3(0.8f, 0.8f, 0.8f));

    moon.setParentSphere(&earth);
    earth.setParentSphere(&sun);

    SetApplicationStartView();

}


/*************************************************************************************************


**************************************************************************************************/
void Universe::printGlError()
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR)
    {
        std::string error_str;

        switch (err) {
        case GL_INVALID_ENUM:                   error_str = "GL_INVALID_ENUM";                   break;
        case GL_INVALID_VALUE:                  error_str = "GL_INVALID_VALUE";                  break;
        case GL_INVALID_OPERATION:              error_str = "GL_INVALID_OPERATION";              break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error_str = "GL_INVALID_FRAMEBUFFER_OPERATION";  break;
        case GL_OUT_OF_MEMORY:                  error_str = "GL_OUT_OF_MEMORY";                  break;
        case GL_STACK_UNDERFLOW:                error_str = "GL_STACK_UNDERFLOW";                break;
        case GL_STACK_OVERFLOW:                 error_str = "GL_STACK_OVERFLOW";                 break;
        }

        printf("Error: %s\n", error_str.c_str());
        err = glGetError();
    }
}






void Universe::advance(float stepMultiplier)
{
    // Advance parents before children
    sun.advance(stepMultiplier);
    earth.advance(stepMultiplier);
    moon.advance(stepMultiplier);
}


/*!
****************************************************************************



****************************************************************************/
void Universe::ChangeSidewaysMotionMode()
{
    bSidewaysMotionMode = !bSidewaysMotionMode;
}

/*!
****************************************************************************



****************************************************************************/
void Universe::SetDefaultView()
{
    space.pushFrame();

    space.initFrame();
    space.rotateFrame(PNT(0, 0, 0), 0, -15.0);
    //space.rotateFrame(PNT(0, 0, 0), 0, 0);

    ResetFollowTargetAndMode();
}

/*!
****************************************************************************



****************************************************************************/
void Universe::SetApplicationStartView()
{
    space.pushFrame();
    space.initFrame();

    Earth_SetOrbitalPositionAngle(3 * M_PI / 8);

    PNT newS = PNT(-266.775859, 2177.232389, 266.224952);
    space.setFrame(AT_POINT,
        newS,
        VECTOR(newS, earth.getCenter()),
        PNT(newS.x, newS.y, newS.z - 100));

    // Adjust navigation view locks on earth and sun
    SetLockTargetAndMode(&earth, TargetLockMode_ViewTarget);
    earth.bRevolutionMotion = true;
}


/*!
****************************************************************************

****************************************************************************/
void Universe::SetLockTargetAndMode(Sphere* target, TargetLockMode mode)
{
    lockTarget = target;

    // follow target has to be set before setting mode.
    SetLockMode(mode);
}

void Universe::SetLockMode(TargetLockMode mode)
{
    lockMode = mode;

    if (lockTarget != nullptr)
    {
        if (mode == TargetLockMode_FollowTarget)
        {
            PNT p = lockTarget->getCenter();
            followVector = VECTOR(space.S, p);
            followDistance = float(space.S.distanceTo(p));
            bSidewaysMotionMode = false;
        }
    }
}

void Universe::ToggleConstantDirectionFollowMode()
{
    if (lockMode == TargetLockMode_FollowTarget)
        SetLockMode(TargetLockMode_ViewTarget);
    else
        SetLockMode(TargetLockMode_FollowTarget);
}

void Universe::ResetFollowTargetAndMode()
{
    SetLockTargetAndMode(nullptr, TargetLockMode_ViewTarget);
}

void Universe::ToggleFollowTarget(Sphere* target, TargetLockMode mode)
{
    if (lockTarget == target)
        lockTarget = nullptr;
    else
        lockTarget = target;

    SetLockMode(mode);
}

void Universe::LookAtTarget()
{
    if (lockMode == TargetLockMode_FollowTarget)
    {
        PNT newS = PNT(lockTarget->getCenter()).translated(-followDistance, followVector);
        space.setFrame(
            AT_POINT,
            newS,
            followVector,
            PNT(newS.x, newS.y, newS.z - 100));
    }
    else
    {
        space.setFrame(
            AT_POINT,
            space.S,
            VECTOR(space.S, lockTarget->getCenter()),
            PNT(space.S.x, space.S.y, space.S.z - 100));
    }
}

/*
 Double the speed each time this is called.
 */
void Universe::IncreaseSimulationSpeed()
{
    if (_stepMultiplier < 400.0f)
        _stepMultiplier *= 2.0f;
}

/*
 Halve the speed each time this is called.
 */
void Universe::DecreaseSimulationSpeed()
{
    if (_stepMultiplier > 0.0001666)
        _stepMultiplier *= 0.5f;
}

/*!
****************************************************************************

 Set simulation speed to one of the 5 hardcoded values.

****************************************************************************/
void Universe::SetSimulationSpeed(int nParam)
{
    eSimulationSpeed = USimulationSpeedType(nParam);
    switch (eSimulationSpeed)
    {
    case USimulationSpeed_1p5625_Percent:      _stepMultiplier = 1.0 / pow(2, 6);        break;
    case USimulationSpeed_3p125_Percent:       _stepMultiplier = 1.0 / pow(2, 5);        break;
    case USimulationSpeed_6p25_Percent:        _stepMultiplier = 1.0 / pow(2, 4);        break;
    case USimulationSpeed_12p5_Percent:        _stepMultiplier = 1.0 / pow(2, 3);        break;
    case USimulationSpeed_25_Percent:          _stepMultiplier = 1.0 / pow(2, 2);        break;
    case USimulationSpeed_50_Percent:          _stepMultiplier = 1.0 / pow(2, 1);        break;
    case USimulationSpeed_100_Percent:         _stepMultiplier = 1.0 / pow(2, 0);        break;
    case USimulationSpeed_400_Percent:         _stepMultiplier = 1.0 / pow(2, -2);        break;
    case USimulationSpeed_1600_Percent:        _stepMultiplier = 1.0 / pow(2, -4);        break;
    }
}

/*!
****************************************************************************

 Set time flow direction to either Forward or Reverse depending on the
 argument.

****************************************************************************/
void Universe::SetTimeDirection(int nParam)
{
    eTimeDirection = (UTimeDirectionType)nParam;
}

/*!
****************************************************************************

 Turn On/Off Fast forward motion.

****************************************************************************/
void Universe::FastForward(int nParam)
{
    ChangeBoolean(&bFastForward, nParam);
}

/*!
****************************************************************************

 Turn On/Off fast Reverse motion.

****************************************************************************/
void Universe::Rewind(int nParam)
{
    ChangeBoolean(&bFastReverse, nParam);
}


/*!
****************************************************************************

 Set, Reset or Toggle a given boolean

 This function operates on a given boolean & sets the boolean to True/False
 or toggles it depending on the second argument which is actually an
 enum of type UCmdParamType.

 Sets bUpdateUI to true unconditionally.

****************************************************************************/
void Universe::ChangeBoolean(bool *pBool, int nParam)
{
    switch (nParam)
    {
    case UCmdParam_Toggle:
        *pBool = !*pBool;
        break;

    case UCmdParam_On:
    case UCmdParam_Start:
        *pBool = true;
        break;

    case UCmdParam_Off:
    case UCmdParam_Stop:
        *pBool = false;
        break;
    }
}


/*!
****************************************************************************



****************************************************************************/
void Universe::SimulationPause(int nParam)
{
    ChangeBoolean(&bSimulationPause, nParam);
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_RotationMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_RevolutionMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    //F_REFERENCE_VECTOR_ALONG_Z = 0;
    //bLockOntoEarth = false;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_PrecessionMotion(int nParam)
{
    if (nParam == UCmdParam_Reset)
    {
        earth.bPrecessionMotion = false;
        earth.setAxisRotationAngle(glm::radians(270.0f));
    }
    else
    {
        ChangeBoolean(&earth.bPrecessionMotion, nParam);
    }
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_OrbitalPlane(int nParam)
{
    ChangeBoolean(&earthRenderer.bShowOrbitalPlane, nParam);
}

void Universe::Earth_Orbit(int nParam)
{
    ChangeBoolean(&earthRenderer.bShowOrbit, nParam);
}
/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_OrbitalPlane(int nParam)
{
    ChangeBoolean(&moonRenderer.bShowOrbitalPlane, nParam);
}


void Universe::Moon_Orbit(int nParam)
{
    ChangeBoolean(&moonRenderer.bShowOrbit, nParam);
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_OrbitalPlaneRotation(int nParam)
{

    if (nParam == UCmdParam_Reset)
    {
        moon.bOrbitalPlaneRotation = false;
        moon._orbitalPlaneRotationAngle = glm::radians(0.0f);
    }
    else
    {
        ChangeBoolean(&moon.bOrbitalPlaneRotation, nParam);
    }
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_RevolutionMotion(int nParam)
{
    ChangeBoolean(&moon.bRevolutionMotion, nParam);
}


/*!
****************************************************************************



****************************************************************************/
void Universe::SetDotDensity(int nParam)
{
    eDotDensity = (UDotDensityType)nParam;
    switch (eDotDensity)
    {
    case UDotDensity_Normal:
        // todo
        //inc_multiplier = 0.8;
        break;

    case UDotDensity_High:
        // todo
        //inc_multiplier = 0.5;
        break;
    }
}



void Universe::toggleFullScreen()
{
    if (bIsWindowFullScreen) {
        SDL_SetWindowFullscreen(window, 0);
        bIsWindowFullScreen = false;
    }
    else {
        printf("  Changing to fullscreen\n");
        //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);        
        bIsWindowFullScreen = true;
    }
}


void Universe::cleanupAndExitApplication()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_SetOrbitalPositionAngle(double fAngle)
{
    moon.setOrbitalAngle(float(fAngle));
    moon.calculateCenterPosition();
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_SetOrbitalPositionAngle(double fAngle)
{
    earth.setOrbitalAngle(float(fAngle));
    earth.calculateCenterPosition();
}


void Universe::setWidgetControlMode()
{
    SDL_ShowCursor(SDL_ENABLE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void Universe::resetWidgetControlMode()
{
    //SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}



/*************************************************************************************************


**************************************************************************************************/
void Universe::processFlags()
{
    navigate(throttle, yaw, pitch, roll);

    if (!bSimulationPause)
    {
        if (bFastForward)
            advance(5 * _stepMultiplier * _stepMultiplierFrameRateAdjustment);
        else if (bFastReverse)
            advance(-5 * _stepMultiplier * _stepMultiplierFrameRateAdjustment);
        else
            advance(_stepMultiplier * _stepMultiplierFrameRateAdjustment);
    }

    if (lockTarget != nullptr)
        LookAtTarget();

}

/*
  Immediately apply the given throttle, yaw, pitch & roll values to the Space frame.
  This changes view only, not anything to do with model.
 
 */
void Universe::navigate(float __throttle, float __yaw, float __pitch, float __roll)
{
    // Amplify or attenuate the value based on keyboard modifiers.
    if (bCtrlModifier)
        __throttle /= 10;
    else if (bShiftModifier)
        __throttle *= 100;

    if (bCtrlModifier)
        __yaw /= 10;
    else if (bShiftModifier)
        __yaw *= 10;

    if (bCtrlModifier)
        __pitch /= 100;
    else if (bShiftModifier)
        __pitch *= 10;

    if (bCtrlModifier)
        __roll /= 10;
    else if (bShiftModifier)
        __roll *= 100;

    if (lockTarget == nullptr)
    {
        // Finally, Apply the motion value.
        if (__throttle != 0.0f)
            space.moveFrame(Movement_Forward, __throttle);

        if ((__yaw != 0.0f) || (__pitch != 0.0f))
        {
            if (bSidewaysMotionMode) {
                space.moveFrame(Movement_RotateRight, 90);
                space.moveFrame(Movement_Forward, __yaw * 50);
                space.moveFrame(Movement_RotateLeft, 90);

                space.moveFrame(Movement_RotateUp, 90);
                space.moveFrame(Movement_Forward, __pitch * 50);
                space.moveFrame(Movement_RotateDown, 90);
            }
            else {
                space.moveFrame(Movement_RotateRight, __yaw);
                space.moveFrame(Movement_RotateUp, __pitch);
            }
        }
        if (__roll != 0.0f)
            space.moveFrame(Movement_RightAlongSD, -__roll);        // todo - why the -ve sign?
    }
    else
    {
        if (lockMode != TargetLockMode_FollowTarget)
        {
            if (__throttle != 0.0f)
                space.moveFrame(Movement_Forward, __throttle);

            // rotate frame about the target's center.
            if ((__yaw != 0.0f) || (__pitch != 0.0f)) {
                float horizontalAngle = -__yaw * 3.0f;
                float verticalAngle = __pitch * 3.0f;
                float currentAngle = space.deg(acos(glm::dot(glm::normalize(space.DS.getGlmVec3()),
                                                      glm::normalize(glm::vec3(0, 0, 1)))));
                
                //printf("__pitch = %f\n", __pitch);
                //printf("currentAngle = %f\n", currentAngle);
                //printf("BEFORE verticalAngle = %f\n", verticalAngle);

                // todo - come up with a better way to make sure angle doesn't get too close to vertical.
                if (__pitch > 0)
                    verticalAngle = glm::min(verticalAngle, currentAngle - 2);
                if (__pitch < 0)
                    if (currentAngle > 178)
                        verticalAngle = 0;
                    else
                        verticalAngle = glm::min(verticalAngle, 180 - currentAngle - 2);

                //printf("AFTER  verticalAngle = %f\n", verticalAngle);
                
                space.rotateFrame(lockTarget->getCenter(), horizontalAngle, verticalAngle);
            }
        }
        else
        {
            if (__throttle != 0.0f)
                followDistance -= __throttle;
        }
    }
}


/*************************************************************************************************


**************************************************************************************************/
int Universe::runMainLoop()
{
	ImGuiIO& io = ImGui::GetIO();
    SDL_Event event;

    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            if (!bMouseGrabbed)
                ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:           bQuit = true;                  break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    setWidgetControlMode();
                    bMouseGrabbed = false;
                }
                else
                    onKeyDown(&event);
                break;
            case SDL_KEYUP:
                onKeyUp(&event);
                break;
            case SDL_WINDOWEVENT:
                if ((event.window.event == SDL_WINDOWEVENT_RESIZED) ||
                    (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
                    curWidth = event.window.data1;
                    curHeight = event.window.data2;
                    glViewport(0, 0, curWidth, curHeight);
                }
                break;
            }

            //----------------------------------------------------
            // Pass mouse events to universe3d application only if ImGui isn't using them.
            if (!io.WantCaptureMouse)
            {
                switch (event.type)
                {
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        if (event.button.clicks == 1) {
                            if (!doubleClicked.get() && !bMouseGrabbed) {
                                resetWidgetControlMode();
                                bMouseGrabbed = true;
                            }
                        }
                        if (event.button.clicks == 2) {
                            if (bMouseGrabbed) {
                                doubleClicked.set(50);
                                setWidgetControlMode();
                                bMouseGrabbed = false;
                            }
                        }
                        bLeftMouseButtonDown = true;
                        break;
                    case SDL_BUTTON_RIGHT:
                        bRightMouseButtonDown = true;
                        break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT:   bLeftMouseButtonDown = false;   break;
                    case SDL_BUTTON_RIGHT:  bRightMouseButtonDown = false;  break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (bMouseGrabbed)
                        onMouseMotion(event.motion.xrel, event.motion.yrel);
                    break;
                case SDL_MOUSEWHEEL:
                    if (bMouseGrabbed) {
                        // Hack
                        bool oldValue = bLeftMouseButtonDown;
                        bLeftMouseButtonDown = true;
                        onMouseMotion(-event.wheel.x * 10, -event.wheel.y * 10);
                        bLeftMouseButtonDown = oldValue;
                    }
                    break;
                }
            }

            if (bQuit)
                break;

        } // while SDL event poll

        generateImGuiWidgets();

        if (bQuit)
            break;

        doubleClicked.tick();
        processFlags();
        render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }

	return 0;

}


/*************************************************************************************************


**************************************************************************************************/
int Universe::run()
{
    setvbuf(stdout, 0, _IONBF, 0);
    const char* glsl_version = "#version 330";

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("Universe3d", 100, 100, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    printf("Created SDL GL window\n");
    context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    glewInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    std::string fullFontFilePath = FindFontFile("Roboto-Medium.ttf");
    appFontExtraSmall   = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 14);
    appFontSmall        = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 16);
    appFontSmallMedium  = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 18);
    appFontMedium       = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 20);
    appFontLarge        = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 24);
    appFontGiant        = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 36);

    fullFontFilePath = FindFontFile("ProggyClean.ttf");
    fixedWidthSmall     = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 13);

    if (!appFontSmall || !appFontSmallMedium || !appFontMedium || !appFontLarge || !fixedWidthSmall)
        printf("WARNING: Could not load fonts.  Will use default fixed width font.");

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    SDL_GetWindowSize(window, &curWidth, &curHeight);
    printf("width = %d\n", curWidth);
    printf("height = %d\n", curHeight);
    
    printf("initializing scene objects... ");

	int retval = 0;
	try
	{
		initSceneObjects();
		initializeGL();
		printf("done\n");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);


		SDL_GetMouseState(&previousX, &previousY);

		/// todo
		runMainLoop();
	}
	catch (exception& e)
	{
        printf("%s\n", e.what());
		retval = 1;
	}


	cout << "Cleaning up..." << endl;
	cleanupAndExitApplication();

    return retval;
}
