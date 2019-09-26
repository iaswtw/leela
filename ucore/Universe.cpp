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
    sunRenderer(sun),
    earthRenderer(earth),
    moonRenderer(moon),
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
    SetDefaultView();
    stars.setCubeStarParameters(
        3500,                               // total number of stars (single + double pixel)
        0.6f                                // radio of single pixel starts to all stars
    );


    axis.setSpan(3600, 3600, 800);
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
        0.02f,                              // rotation velocity
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
        0.005f,                             // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(10.0f)                 // axis tilt angle
    );
    moon.setOrbitalParameters(360,          // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                              // revolution velocity
        0,                                  // orbital rotation angle
        glm::radians(30.0f)                 // orbital tilt
    );
    moon.setOrbitalPlaneColor(glm::vec3(0.8f, 0.8f, 0.8f));

    moon.setParentSphere(&earth);
    earth.setParentSphere(&sun);


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


/*************************************************************************************************


**************************************************************************************************/
void Universe::initializeGL()
{
    printf("Inside initializeGL\n");

	std::string vertFilename("../ucore/shaders/planet.vert.glsl");
	std::string fragFilename("../ucore/shaders/planet.frag.glsl");
	planetGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
	planetGlslProgram.link();


	vertFilename = "../ucore/shaders/sun.vert.glsl";
	fragFilename = "../ucore/shaders/sun.frag.glsl";
	sunGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
	sunGlslProgram.link();

	vertFilename = "../ucore/shaders/star.vert.glsl";
	fragFilename = "../ucore/shaders/star.frag.glsl";
	starGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
	starGlslProgram.link();

    vertFilename = "../ucore/shaders/simple.vert.glsl";
    fragFilename = "../ucore/shaders/simple.frag.glsl";
    simpleGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
    simpleGlslProgram.link();


    //---------------------------------------------------------------------------------------------------
    // Axis
    axisRenderer.constructVerticesAndSendToGpu();
    starsRenderer.constructVerticesAndSendToGpu();
    
    //---------------------------------------------------------------------------------------------------

	sunRenderer.setAsLightSource();
    sunRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    sunRenderer.constructVerticesAndSendToGpu();

    
    earthRenderer.setPolygonCountLevel(PolygonCountLevel_High);
    earthRenderer.constructVerticesAndSendToGpu();
    earthRenderer.bShowLatitudesAndLongitudes = true;
    earthRenderer.setNightColorDarkness(NightColorDarkness_Medium);

    
    moonRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
    moonRenderer.constructVerticesAndSendToGpu();
    moonRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);

    glBindVertexArray(0);       // Disable VBO
}




void Universe::advance(float stepMultiplier)
{
    // Advance parents before children
    sun.advance(stepMultiplier);
    earth.advance(stepMultiplier);
    moon.advance(stepMultiplier);
}

void Universe::onKeyDown(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_a:
        ChangeBoolean(&bShowAxis, UCmdParam_Toggle);
        break;
    case SDLK_b:
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Toggle);
        break;
    case SDLK_c:
        NavigationLockOntoSun(UCmdParam_Toggle);
        break;
    case SDLK_d:
        SetDefaultView();
        break;
    case SDLK_e:
        Earth_OrbitalPlane(UCmdParam_Toggle);
        break;
    case SDLK_f:
        FastForward(UCmdParam_Start);
        break;
    case SDLK_m:
        Moon_OrbitalPlane(UCmdParam_Toggle);
        break;
    case SDLK_p:
    case SDLK_PAUSE:
    case SDLK_SPACE:
        SimulationPause(UCmdParam_Toggle);
        break;
    case SDLK_r:
        Rewind(UCmdParam_Start);
        break;
    case SDLK_s:
        ChangeBoolean(&bGalaxyStars, UCmdParam_Toggle);
        break;
    case SDLK_v:
        ChangeSidewaysMotionMode();
        break;
    case SDLK_z:
        NavigationLockOntoEarth(UCmdParam_Toggle);
        break;

    case SDLK_0:
        Earth_RevolutionMotion(UCmdParam_Toggle);
        break;
    case SDLK_1:
        Earth_SetOrbitalPositionAngle(0);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_2:
        Earth_SetOrbitalPositionAngle(M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_3:
        Earth_SetOrbitalPositionAngle(M_PI);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_4:
        Earth_SetOrbitalPositionAngle(3 * M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;



    // Function keys
    case SDLK_F6:
        if (bShiftModifier)
            Earth_PrecessionMotion(UCmdParam_Reset);
        else
            Earth_PrecessionMotion(UCmdParam_Toggle);
        break;
    case SDLK_F5:
        if (bShiftModifier)
            Moon_OrbitalPlaneRotation(UCmdParam_Reset);
        else
            Moon_OrbitalPlaneRotation(UCmdParam_Toggle);
        break;
    case SDLK_F11:
        toggleFullScreen();
        break;

    // Arrow keys
    case SDLK_UP:
        _stepMultiplier *= 1.6666f;
        break;
    case SDLK_DOWN:
        _stepMultiplier *= 0.6f;
        break;


    case SDLK_HOME:
        throttle = nominalThrottle;
        break;
    case SDLK_END:
        throttle = -nominalThrottle;
        break;
    case SDLK_PAGEDOWN:
        yaw = nominalYaw;
        break;
    case SDLK_DELETE:
        yaw = -nominalYaw;
        break;
    case SDLK_PAGEUP:
        if (!bAltModifier)
            pitch = nominalPitch;
        else
            roll = -nominalRoll;
        break;
    case SDLK_INSERT:
        if (!bAltModifier)
            pitch = -nominalPitch; 
        else
            roll = nominalRoll;
        break;

    // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        bCtrlModifier = true;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        bShiftModifier = true;
        break;
    case SDLK_LALT:
    case SDLK_RALT:
        bAltModifier = true;
        break;

    }

}

void Universe::onKeyUp(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_f:
        FastForward(UCmdParam_Stop);
        break;
    case SDLK_r:
        Rewind(UCmdParam_Stop);
        break;

    case SDLK_HOME:
    case SDLK_END:
        throttle = noThrottle;
        break;
    case SDLK_PAGEDOWN:
    case SDLK_DELETE:
        yaw = noYaw;
        break;
    case SDLK_PAGEUP:
    case SDLK_INSERT:
        roll = noRoll;
        pitch = noPitch;
        break;


        // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        bCtrlModifier = false;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        bShiftModifier = false;
        break;
    case SDLK_LALT:
    case SDLK_RALT:
        bAltModifier = false;
        break;

    }

}

void Universe::onMouseMotion(int xrel, int yrel)
{
    float dx = float(xrel);
    float dy = float(yrel);

    if (bCtrlModifier)
    {
        dx /= 15.0f;
        dy /= 15.0f;
    }

    //printf("dx = %d, dy = %d\n", dx, dy);

    if (!bEarthFollowMode) {
        if (F_REFERENCE_VECTOR_ALONG_Z == 1)
        {
            if (bLeftMouseButtonDown) {
                space.moveFrame(Movement_Forward, -dy * 5);
                if (bLockOntoEarth)
                    space.rotateFrame(earth.getCenter(), dx / 10., 0);
                else if (bLockOntoSun)
                    space.rotateFrame(sun.getCenter(), dx / 10., 0);
            }
            else {
                if (bLockOntoEarth)
                    space.rotateFrame(earth.getCenter(), dx / 10., -dy / 10.);
                else if (bLockOntoSun)
                    space.rotateFrame(sun.getCenter(), dx / 10., -dy / 10.);
            }
            return;
        }
    }

    while (1)
    {
        if (bLeftMouseButtonDown) {
            space.moveFrame(Movement_Forward, -dy * 5);

            if (bEarthFollowMode)
                earthFollowDistance += dy * 5;

            if (!bEarthFollowMode) {
                if (bSidewaysMotionMode == 1) {
                    space.moveFrame(Movement_RotateRight, 90);
                    space.moveFrame(Movement_Forward, dx);
                    space.moveFrame(Movement_RotateLeft, 90);
                }
                else {
                    space.moveFrame(Movement_RotateRight, dx / 10.0);
                }
            }
            break;
        }
        if (!bEarthFollowMode) {
            if (bRightMouseButtonDown) {
                space.moveFrame(Movement_RightAlongSD, dx / 10.0);
                break;
            }

            if (bSidewaysMotionMode) {
                space.moveFrame(Movement_RotateRight, 90);
                space.moveFrame(Movement_Forward, dx);
                space.moveFrame(Movement_RotateLeft, 90);

                space.moveFrame(Movement_RotateUp, 90);
                space.moveFrame(Movement_Forward, -dy);
                space.moveFrame(Movement_RotateDown, 90);

            }
            else {
                space.moveFrame(Movement_RotateRight, (dx / 10.0));
                space.moveFrame(Movement_RotateUp, -(dy / 10.0));
            }
        }
        break;
    }
}


/*!
****************************************************************************



****************************************************************************/
void Universe::ChangeSidewaysMotionMode()
{
    bSidewaysMotionMode = !bSidewaysMotionMode;
    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Universe::SetDefaultView()
{
    space.pushFrame();

    space.initFrame();
    space.rotateFrame(PNT(0, 0, 0), -10.0, -15.0);
    //space.moveFrame(Movement_Backward, 3400);

    bLockOntoSun = false;
    bLockOntoEarth = false;
    F_REFERENCE_VECTOR_ALONG_Z = 0;

    bUpdateUI = true;
}

/*!
****************************************************************************

 Lock/Unlock/Toggle the earth at the center of the view.  Set the
 reference vector along -Z.

****************************************************************************/
void Universe::NavigationLockOntoEarth(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bLockOntoEarth = true;
        bLockOntoSun = false;
        F_REFERENCE_VECTOR_ALONG_Z = 1;

        ChangeBoolean(&earth.bRevolutionMotion, UCmdParam_Off);
        LookAtEarth();
        break;

    case UCmdParam_Off:
        bLockOntoEarth = false;
        F_REFERENCE_VECTOR_ALONG_Z = 0;
        break;

    case UCmdParam_Toggle:
        if (bLockOntoEarth)
        {
            NavigationLockOntoEarth(UCmdParam_Off);
        }
        else
        {
            NavigationLockOntoEarth(UCmdParam_On);
        }
        break;
    }

    bUpdateUI = true;

}

/*!
****************************************************************************

 Lock/Unlock/Toggle the sun at the center of the view.  Set the
 reference vector along -Z.

****************************************************************************/
void Universe::NavigationLockOntoSun(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bLockOntoSun = true;
        bLockOntoEarth = false;
        F_REFERENCE_VECTOR_ALONG_Z = 1;
        LookAtSun();
        break;

    case UCmdParam_Off:
        bLockOntoSun = false;
        F_REFERENCE_VECTOR_ALONG_Z = 0;
        break;

    case UCmdParam_Toggle:
        if (bLockOntoSun)
        {
            NavigationLockOntoSun(UCmdParam_Off);
        }
        else
        {
            NavigationLockOntoSun(UCmdParam_On);
        }
        break;
    }

    bUpdateUI = true;

}

void Universe::NavigationLockOntoEarthWithConstantDirection(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bEarthFollowMode = true;
        break;

    case UCmdParam_Off:
        bEarthFollowMode = false;
        break;

    case UCmdParam_Toggle:
        ChangeBoolean(&bEarthFollowMode, UCmdParam_Toggle);
        break;
    }

    if (bEarthFollowMode)
    {
        bLockOntoSun = false;
        bLockOntoEarth = false;

        earthFollowVector = VECTOR(space.S, earth.getCenter());
        PNT p = earth.getCenter();
        earthFollowDistance = float(space.S.distanceTo(p));
        bSidewaysMotionMode = false;
    }
    else
    {
        F_REFERENCE_VECTOR_ALONG_Z = 0;
    }

    bUpdateUI = true;
}



void Universe::LookAtEarth()
{
    space.setFrame(AT_POINT,
        space.S,
        VECTOR(space.S, earth.getCenter()),
        PNT(space.S.x, space.S.y, space.S.z - 100));

}

void Universe::LookAtEarthFromSavedVector()
{
    space.setFrame(AT_POINT,
        PNT(earth.getCenter()).translated(-earthFollowDistance, earthFollowVector),
        earthFollowVector,
        PNT(space.S.x, space.S.y, space.S.z - 100));

}

void Universe::LookAtSun()
{
    space.setFrame(AT_POINT,
        space.S,
        VECTOR(space.S, sun.getCenter()),
        PNT(space.S.x, space.S.y, space.S.z - 100));

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
    case USimulationSpeed_VeryLow:
        _stepMultiplier = 0.005f;
        break;

    case USimulationSpeed_Low2:
        _stepMultiplier = 0.015f;
        break;

    case USimulationSpeed_Low1:
        _stepMultiplier = 0.03f;
        break;

    case USimulationSpeed_Low:
        _stepMultiplier = 0.05f;
        break;

    case USimulationSpeed_Normal:
        _stepMultiplier = 0.36f;
        break;

    case USimulationSpeed_High:
        _stepMultiplier = 3.0f;
        break;

    case USimulationSpeed_VeryHigh:
        _stepMultiplier = 10.0f;
        break;
    }

    bUpdateUI = true;
}

/*!
****************************************************************************

 Set time flow direction to either Forward or Reverse depending on the
 argument.

****************************************************************************/
void Universe::SetTimeDirection(int nParam)
{
    eTimeDirection = (UTimeDirectionType)nParam;
    bUpdateUI = true;
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

    bUpdateUI = true;

}


/*!
****************************************************************************



****************************************************************************/
void Universe::SimulationPause(int nParam)
{
    ChangeBoolean(&bSimulationPause, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_RotationMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_RevolutionMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    //F_REFERENCE_VECTOR_ALONG_Z = 0;
    //bLockOntoEarth = false;

    bUpdateUI = true;
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

    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_OrbitalPlane(int nParam)
{
    ChangeBoolean(&earthRenderer.bShowOrbitalPlane, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_OrbitalPlane(int nParam)
{
    ChangeBoolean(&moonRenderer.bShowOrbitalPlane, nParam);
    bUpdateUI = true;

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

    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_RevolutionMotion(int nParam)
{
    ChangeBoolean(&moon.bRevolutionMotion, nParam);
    bUpdateUI = true;
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


/*!
****************************************************************************



****************************************************************************/
void Universe::ShowDemo(int nParam)
{
    PNT newS;

    switch ((UDemoType)nParam)
    {
    case UDemo_TotalSolarEclipse:

        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(-323.5104, 957.3688, 296.3317);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.2f * M_PI / 5);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low1);
        SimulationPause(UCmdParam_Off);

        /* F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
           or bLockOntoSun in the function on_MouseMotion().  Need
           to consider if the priority of this check should be reversed.
           Without the setting below, the Lock on earth or sun won't
           work. */
        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;

        break;
    
    case UDemo_TotalSolarEclipseOnNorthPole:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(733.4838, 817.9659, 297.3985);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.7f * M_PI / 5);
        Earth_SetOrbitalPositionAngle(0.69f * M_PI / 2);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low);
        SimulationPause(UCmdParam_Off);

        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;
        break;

    case UDemo_AnnularSolarEclipseFromSpace:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_Off);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        // the hardcoded values here were found by printing the value of S & D on screen using ImGui
        // while manually going to that position.
        newS = PNT(-90.1736, 1423.7947, 51.6696);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, PNT(1220.0681, -2517.4311, -572.8346)),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.3f * M_PI / 5);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low2);
        SimulationPause(UCmdParam_Off);

        F_REFERENCE_VECTOR_ALONG_Z = 0;
        bSidewaysMotionMode = false;

        bUpdateUI = true;
        break;

    case UDemo_PartialLunarEclipse:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(1196.26, 462.93, -60.55);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(0.35f * M_PI / 2);
        Earth_SetOrbitalPositionAngle(0.59f * M_PI / 2);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low);
        SimulationPause(UCmdParam_Off);

        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;
        break;

    case UDemo_PrecessionMotion:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        // Let the moon hide below the earth's orbital plane so that
        // it won't distract the user
        Moon_SetOrbitalPositionAngle(0);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        newS = PNT(earth.getCenter().x + 300, earth.getCenter().y - 400, earth.getCenter().z + 300);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));


        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_Off);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Start);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_Off);

        // Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_Normal);
        SimulationPause(UCmdParam_Off);

        /* F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
           or bLockOntoSun in the function on_MouseMotion().  Need
           to consider if the priority of this check should be reversed.
           Without the setting below, the Lock on earth or sun won't
           work. */
        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;

        break;

    case UDemo_SixMonthLongDayAndNightOnNorthPole:

        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        // Set S
        newS = PNT(earth.getCenter().x + 250, earth.getCenter().y - 350, earth.getCenter().z + 300);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        NavigationLockOntoEarthWithConstantDirection(UCmdParam_On);
        // Adjust navigation view locks on earth and sun

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        //// Adjust Moon's motions
        //Moon_RevolutionMotion(UCmdParam_Off);

        //// Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_High);
        SimulationPause(UCmdParam_Off);

        ///* F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
        //   or bLockOntoSun in the function on_MouseMotion().  Need
        //   to consider if the priority of this check should be reversed.
        //   Without the setting below, the Lock on earth or sun won't
        //   work. */
        //F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;

        break;

    case UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle:

        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI + M_PI / 4);

        // Set S
        newS = PNT(earth.getCenter().x, earth.getCenter().y - 350, earth.getCenter().z + 100);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        NavigationLockOntoEarthWithConstantDirection(UCmdParam_On);
        // Adjust navigation view locks on earth and sun

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        //// Adjust Moon's motions
        //Moon_RevolutionMotion(UCmdParam_Off);

        //// Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_High);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_TiltedOrbitalPlanes:
        Earth_SetOrbitalPositionAngle(M_PI / 2 - M_PI / 10);

        // Tilted orbit demo must show both the orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_On);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_Off);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        newS = PNT(earth.getCenter().x + 0, earth.getCenter().y + 1300, earth.getCenter().z + 200);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, PNT(-700, 0, -300)),
            PNT(newS.x, newS.y, newS.z - 100));


        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);
        Moon_SetOrbitalPositionAngle(0.0f);

        SetSimulationSpeed(USimulationSpeed_Normal);
        SetDotDensity(UDotDensity_Normal);
        SimulationPause(UCmdParam_Off);


        bUpdateUI = true;

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
void Universe::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //=====================================================================================
    // Create View and projection that remain the same for the entire scene
    //=====================================================================================
    // View transformation
    //----------------------------------------------
    // Create the initial View matrix
    viewMatrix = glm::lookAt(
        space.getSourcePoint(),
        space.getDirectionPoint(),
        space.getUpwardDirectionVector());

	// perspective transformation
	//----------------------------------------------
	projectionMatrix = glm::perspective(
		glm::radians(35.0f),
		float(curWidth) / float(curHeight),
		1.0f,
		10000000.0f);
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    planetGlslProgram.use();
    planetGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    planetGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

	renderUsingPlanetGlslProgram();

    planetGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    starGlslProgram.use();
    printGlError();
    starGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    starGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));
    
    renderUsingStarGlslProgram();

    starGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    sunGlslProgram.use();
    sunGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    sunGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

	renderUsingSunGlslProgram();

    sunGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    simpleGlslProgram.use();
    simpleGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    simpleGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

    renderUsingSimpleGlslProgram();

    simpleGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    glBindVertexArray(0);

}

void Universe::renderUsingPlanetGlslProgram()
{
    planetGlslProgram.setVec3("sunCenterTransformed", glm::value_ptr(sun.getModelTransformedCenter()));
    planetGlslProgram.setFloat("sunRadius", sun.getRadius());

    earthRenderer.renderSphere(planetGlslProgram , &sun, &moon);
    earthRenderer.renderLatitudeAndLongitudes(planetGlslProgram);

    moonRenderer.renderSphere(planetGlslProgram , &sun, &earth);
    moonRenderer.renderLatitudeAndLongitudes(planetGlslProgram);

}

void Universe::renderUsingStarGlslProgram()
{
    if (!bGalaxyStars)
    {
        starsRenderer.renderCubeStars(starGlslProgram);
    }
    else
    {
        starsRenderer.renderGalaxyStars(starGlslProgram);
    }
}

void Universe::renderUsingSunGlslProgram()
{
    sunRenderer.renderSphere(sunGlslProgram);
}

void Universe::renderUsingSimpleGlslProgram()
{
    if (bShowAxis)
    {
        axisRenderer.render(simpleGlslProgram);
    }

    earthRenderer.renderOrbitalPlane(simpleGlslProgram);
    earthRenderer.renderOrbit(simpleGlslProgram);

    moonRenderer.renderOrbitalPlane(simpleGlslProgram);
    moonRenderer.renderOrbit(simpleGlslProgram);
}


/*************************************************************************************************


**************************************************************************************************/
void Universe::processFlags()
{
    // Accept the current values of the 4 motions into local variables
    float __throttle = throttle;
    float __yaw = yaw;
    float __pitch = pitch;
    float __roll = roll;
    

    // Amply or attenuate the value based on keyboard modifiers.
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

    // Finally, Apply the motion value.
    if (__throttle != 0.0f) {
        space.moveFrame(Movement_Forward, __throttle);
        if (bEarthFollowMode)
            earthFollowDistance -= __throttle;
    }

    if ((__yaw != 0.0f) || (__pitch != 0.0f))
    {
        if (bSidewaysMotionMode) {
            space.moveFrame(Movement_RotateRight, 90);
            space.moveFrame(Movement_Forward, __yaw*50);
            space.moveFrame(Movement_RotateLeft, 90);

            space.moveFrame(Movement_RotateUp, 90);
            space.moveFrame(Movement_Forward, __pitch*50);
            space.moveFrame(Movement_RotateDown, 90);
        }
        else {
            space.moveFrame(Movement_RotateRight, __yaw);
            space.moveFrame(Movement_RotateUp, __pitch);
        }
    }
    if (__roll != 0.0f)
        space.moveFrame(Movement_RightAlongSD, -__roll);        // todo - why the -ve sign?



    if (!bSimulationPause)
    {
        if (bFastForward)
            advance(5 * _stepMultiplier);
        else if (bFastReverse)
            advance(-5 * _stepMultiplier);
        else
            advance(_stepMultiplier);
    }


    if (bLockOntoEarth)
        LookAtEarth();
    if (bLockOntoSun)
        LookAtSun();
    if (bEarthFollowMode)
        LookAtEarthFromSavedVector();


}


void Universe::generateImGuiWidgets()
{
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    // Always showing overlay window showing status of various flags
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(5.0f, io.DisplaySize.y - 5.0f), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Flags", &bShowFlagsOverlay, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMouseInputs))
    {

        ImVec4 onColor  = ImColor(0.3f, 0.7f, 0.3f);
        ImVec4 offColor = ImColor(0.0f, 0.0f, 0.0f);
        ImVec4 color;

        if (bSidewaysMotionMode) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("v"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (bLockOntoEarth) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("z"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (!earth.bRevolutionMotion) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("0"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (bSimulationPause) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("Pause"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (earthRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("e"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (moonRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("m"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (bEarthFollowMode) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("b");
        ImGui::PopStyleColor();

    }
    ImGui::End();

    if (bShowIntroduction)
    {
        float width = 800.0f;
        float height = 600.0f;
        //float x = io.DisplaySize.x - (width / 2);
        //float y = io.DisplaySize.y - (height / 2);
       
        ImGui::SetNextWindowSizeConstraints(ImVec2(width, height), ImVec2(width, height));
        //ImGui::SetNextWindowPos(ImVec2(, y), 0, ImVec2(0.5, 0.5));
        //ImGui::SetCursorPos(ImVec2(io.DisplaySize.x / 2.0f, io.DisplaySize.y / 2.0f));
        if (ImGui::Begin("Introduction", &bShowIntroduction))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1));

            ImGui::PushFont(appFontGiant);
            ImGui::TextWrapped("Universe3d\n\n");
            ImGui::PopFont();

            ImGui::PushFont(appFontMedium);
            ImGui::TextWrapped(
                "This program is intended to be a teaching aid to explain various celestial concepts. "
                "It primarily intends to explain phenomenon related to orbital mechanics of planetary motion. \n\n"
                
                "The program doesn't make any effort to show the various parameters such as sizes, distances, rotation & revolution speeds, etc. to scale. "
                "If it did, it will hinder its ability to help explain concepts due to the impracticality of showing the earth, moon and sun on the screen at "
                "the same time. They will only be a few pixels wide.\n\n"

                "With distances and sizes not to scale, angles have to be exagerated to show the same effect.  For example, to show why solar eclipses don't occurr on every "
                "new moon day, the tilt of the moons's orbital plane with respect to earth's orbital plane had to be exagerated to roughly 30 degrees from about 5 degrees."
                            
            );
            ImGui::PopFont();

            ImGui::PopStyleColor();
        }
        ImGui::End();
    }

    if (bShowKeyboardShortcuts)
    {
        //ImGuiCond_FirstUseEver
        ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(1024, 768));
        if (ImGui::Begin("Keyboard Shortcuts", &bShowKeyboardShortcuts))
        {
            auto populateShortcutWindow = [](const char *arr[][2], int numRows) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1));
                for (int i = 0; i < numRows; i++)
                {
                    if (arr[i][0] == nullptr) {
                        ImGui::Separator();
                    }
                    else {
                        ImGui::Text(arr[i][0]);  ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 100);
                        ImGui::TextWrapped(arr[i][1]);  ImGui::NextColumn();
                    }
                }
                ImGui::PopStyleColor();
            };

            const char * shortcutHelp[][2] = {
            { "Space bar",      "Pause time. Navigation will still work." },
            { "f",              "Pressing this key and keeping it pressed will cause simulation time to elapse faster by about 10 times."},
            { "r",              "Pressing this key and keeping it pressed with cause simulation time to run in reverse by about 10 times the nominal forward speed. "
                                "Releasing this key shall restore the forward movement of time at the speed it was just before pressing this key."},
            { "Up arrow",       "Speeds up the simulation time passage by roughly 66% each time this key is pressed."},
            { "Down arrow",     "Slows down the simulation time passage by roughly 66% each time this key is pressed."},
            { "d",              "Bring the camera to the default position and look in the direction of the sun. From this position, the entire earth's orbit is visible. "
                                " It is a convenient starting position for the simulation.  If you get lost navigating, you can hit this key to get your bearings."},
            { nullptr, nullptr },

            { "a",              "Toggle visibility of the XYZ coordinate axis.  When looking from the default view position, +ve X direction is towards bottom left from origin (blue),"
                                "+ve Y is towards bottom right from origin (green), and +ve Z is upwards from origin (cyan)." },
            { "e",              "Toggle visibility of earth's orbital plane." },
            { "m",              "Toggle visibility of moon's orbital plane." },
            
            { "z",              "Toggle lock on earth's position. When turned on, the earth will appear at the center of the screen. Left/right/up/down mouse movements shall rotate the camera around the earth at a constant distance. "
                                "Zoom in and zoom out will work as expected.  Turning lock on will also cause the earth to pause in its orbit." },
            { "c",              "Toggle lock on sun's position. When turned on, the sun will appear at the center of the screen." },
            { "b",              "Toggle directional lock on earth. The direction vector from the camera to the earth will be noted at the time of turning this mode on. "
                                "After that, the camera will follow the earth as the earth moves in its orbit, all the while maintaining the original direction vector."
                                "This mode is used in the 6 month long day and night demo.  Use this mode when you want to observe the earth from a fixed angle as it "
                                "revolves around the sun.  Contrast this mode with the normal lock on earth (z) mode where the camera stays put while only changing its viewing direction. "
                                "While in this mode, only zoom in/out navigation will work." },
            { "0 (zero)",       "Toggle earth's revolution motion.  Use this to pause the earth in its orbit so that you can observe various things such as the tilted orbit, shadows, etc." },


            { nullptr, nullptr },

            { "Home",           "Zoom in" },
            { "End",            "Zoom out" },
            { "Del",            "Turn left (or shift left in Shift mode)"},
            { "Page Down",      "Turn right (or shift right in Shift mode)"},
            { "Page Up",        "Rotate up (or shift up in Shift mode)"},
            { "Insert",         "Rotate doen (or shift down in Shift mode"},
            { "Alt + Page Up",  "Rotate right along the axis the camera is looking at. This will result in the object in front being rotated left."},
            { "Alt + Insert",   "Rotate left along the axis the camera is looking at. This will result in the object in front being rotated right."},
            { "v",              "Toggle 'Shift mode' navigation.  In this mode, left/right/up/down mouse movements will result in the camera being shifted in those directions as "
                                "as opposed to rotating in those directions.  When the camera shifts, the viewing direction vector does not change. It is as if the camera is looking at a point at infinity. "
                                "If you want to look at how the background stars shift due to parallex as the earth moves in its orbit, turn this mode on, go to default "
                                "view (d), and then move the mouse left and right." },


            { nullptr, nullptr },

            { "F5",             "Start/Stop rotation of moon's orbital plane. This is best seen if moon's orbital plane is also visible." },
            { "Shift + F5",     "Reset moon's orbital plane tilt to default." },

            { "F6",             "Start/Stop earth's precession motion." },
            { "Shift + F6",     "Reset earth's axis tilt direction to default." },

            { nullptr, nullptr },

            { "1",              "Set earth's position at 0 degrees measured from the positive X axis (blue)." },
            { "2",              "Set earth's position at 90 degrees measured from the positive X axis (blue)." },
            { "3",              "Set earth's position at 180 degrees measured from the positive X axis (blue)." },
            { "4",              "Set earth's position at 270 degrees measured from the positive X axis (blue)." },

            {nullptr, nullptr },

            };

            ImGui::Columns(2, "mycolumn");
            ImGui::Text("Shortcut"); ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 100);
            ImGui::Text("Description"); ImGui::NextColumn();
            ImGui::Separator();

            populateShortcutWindow(shortcutHelp, sizeof(shortcutHelp) / sizeof(shortcutHelp[0]));
        }
        ImGui::End();

    }


    if (bMouseGrabbed)
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2.0f, 10.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("Escape message", &bShowFlagsOverlay, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("Escape key / double-click to get mouse back");
        }
        ImGui::End();
    }
    else
    {
        // Show menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Universe3d"))
            {
                if (ImGui::MenuItem("Exit Application"))
                    bQuit = true;

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Show Fullscreen", "F11"))
                    toggleFullScreen();
                ImGui::MenuItem("Show Control Panel in Navigation mode", nullptr, &bAlwaysShowControlPanel, true);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem("Introduction", nullptr, &bShowIntroduction);
                ImGui::MenuItem("Show Keyboard Shortcuts", nullptr, &bShowKeyboardShortcuts);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (!bMouseGrabbed || bAlwaysShowControlPanel)
    {
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 5.0f, 27.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        //ImGui::SetNextWindowSize(ImVec2(350.0f, curHeight - 25.0f));
        ImGui::SetNextWindowBgAlpha(0.8f);

        {
            // Create a window called "Hello, world!" and append into it.
            ImGui::Begin(
                "Control Panel",
                nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
            );
            static float f = 0.0f;
            ImGui::PushFont(appFontSmall);
            if (ImGui::CollapsingHeader("Demos", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::PushFont(appFontSmall);

                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Total Solar Eclipse## demo"))
                    ShowDemo(UDemo_TotalSolarEclipse);

                if (ImGui::Button("Total Solar Eclipse on north pole## demo"))
                    ShowDemo(UDemo_TotalSolarEclipseOnNorthPole);

                if (ImGui::Button("Annular Solar Eclipse from space## demo"))
                    ShowDemo(UDemo_AnnularSolarEclipseFromSpace);
                ImGui::SameLine();
                HelpMarker("This also shows umbra starting to travel over the earth near the bottom left of the screen. This happens roughly halfway between 3rd and 4th contact for a duration of about 2 to 3 seconds. So watch out!");

                if (ImGui::Button("Partial Lunar Eclipse## demo"))
                    ShowDemo(UDemo_PartialLunarEclipse);

                if (ImGui::Button("Tilted orbital planes## demo"))
                    ShowDemo(UDemo_TiltedOrbitalPlanes);

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole);

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole (another angle)"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle);

                if (ImGui::Button("Precession motion## demo"))
                    ShowDemo(UDemo_PrecessionMotion);

                ImGui::PopFont();
            }
            ImGui::PopFont();
            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Earth:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();
            ImGui::Indent();
            ImGui::Checkbox("Revolution motion (0)## earth", &earth.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (e)## earth", &earthRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Precession motion (F6)## earth", &earth.bPrecessionMotion);
            ImGui::SameLine();
            if (ImGui::Button("Reset## earth precession motion"))
                Earth_PrecessionMotion(UCmdParam_Reset);
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Moon:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Revolution motion", &moon.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (m)##moon", &moonRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Orbital plane rotation (F5)", &moon.bOrbitalPlaneRotation);
            ImGui::SameLine();
            if (ImGui::Button("Reset## moon orbital plane rotation"))
                Moon_OrbitalPlaneRotation(UCmdParam_Reset);
            ImGui::Unindent();
            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Navigation:");
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Shift mode (v)", &bSidewaysMotionMode);
            ImGui::SameLine(); HelpMarker("When checked, Shift left/right/up/down on mouse movements.\nWhen unchecked, rotate instead.");
            ImGui::Checkbox("Lock on earth (z)", &bLockOntoEarth);
            if (ImGui::IsItemEdited())
                NavigationLockOntoEarth(bLockOntoEarth ? UCmdParam_On : UCmdParam_Off);
            ImGui::SameLine(); HelpMarker("Also pauses earth's revolution. Activate this mode and\nthen use mouse to view earth from different angles.");

            ImGui::Checkbox("Directional lock on earth (b)", &bEarthFollowMode);
            if (ImGui::IsItemEdited())
                NavigationLockOntoEarthWithConstantDirection(bEarthFollowMode ? UCmdParam_On : UCmdParam_Off);
            ImGui::SameLine();
            HelpMarker("Earth follow mode");

            ImGui::Checkbox("Lock on sun (c)", &bLockOntoSun);
            if (ImGui::IsItemEdited())
                NavigationLockOntoSun(bLockOntoSun ? UCmdParam_On : UCmdParam_Off);

            if (ImGui::Button("Show default view (d)"))
                SetDefaultView();
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Time:");
            ImGui::PopFont();
            ImGui::Indent();

            ImGui::Checkbox("Time pause (space bar)", &bSimulationPause);

            ImGui::Button("Fast Rewind (r)");
            if (ImGui::IsItemActivated())       Rewind(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     Rewind(UCmdParam_Off);

            ImGui::SameLine();
            ImGui::Button("Fast Forward (f)");
            if (ImGui::IsItemActivated())       FastForward(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     FastForward(UCmdParam_Off);

            static int timeSpeed = 0;
            ImGui::SetNextItemWidth(100);
            ImGui::Combo("Time Speed", &timeSpeed, "Very Slow\0Slow\0Normal\0Fast\0Very Fast\0\0");
            // todo - actually apply this value in the app

            ImGui::Unindent();

            //-----------------------------------------------------

            ImGui::Separator();
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Separator();
            ImGui::Text("S: %.4f, %.4f, %.4f", space.S.x, space.S.y, space.S.z);
            ImGui::Text("D: %.4f, %.4f, %.4f", space.D.x, space.D.y, space.D.z);
            ImGui::Text("E orbital angle: %.4f", earth._orbitalAngle);
            ImGui::End();
        }

        // 3. Show another simple window.
        //if (show_another_window)
        //{
        //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //    ImGui::Text("Hello from another window!");
        //    if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //    ImGui::End();
        //}

        // Rendering
    }

    ImGui::Render();
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
		retval = 1;
	}


	cout << "Cleaning up..." << endl;
	cleanupAndExitApplication();

    return retval;
}
