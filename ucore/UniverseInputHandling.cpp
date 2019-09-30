#include "pch.h"
#include "Universe.h"

void Universe::onKeyDown(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_a:
        ChangeBoolean(&bShowAxis, UCmdParam_Toggle);
        break;
    case SDLK_b:
        ToggleConstantDirectionFollowMode();
        break;
    case SDLK_c:
        ToggleFollowTarget(&sun, FollowMode_Normal);
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
    case SDLK_o:
        ChangeBoolean(&bShowOrbitsGlobalEnable, UCmdParam_Toggle);
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
        if (bShiftModifier && bCtrlModifier && bAltModifier)
        {
            char buffer[100];
            std::snprintf(buffer, 100, "%f, %f, %f", space.S.x, space.S.y, space.S.z);
            logString = buffer;
        }
        else
        {
            ChangeBoolean(&bGalaxyStars, UCmdParam_Toggle);
        }
        break;
    case SDLK_v:
        ChangeSidewaysMotionMode();
        break;
    case SDLK_z:
        ToggleFollowTarget(&earth, FollowMode_Normal);
        break;

    // ============= Digits ===============
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



    // ============ Function keys =============
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
    case SDLK_LEFT:
        DecreaseSimulationSpeed();
        break;
    case SDLK_RIGHT:
        IncreaseSimulationSpeed();
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

    float l_throttle  = 0.0f;
    float l_yaw       = 0.0f;
    float l_pitch     = 0.0f;
    float l_roll      = 0.0f;

    if (bLeftMouseButtonDown)
        l_throttle = -dy * 5.0f;
    else
        l_pitch = -dy / 20.0f;

    if (bRightMouseButtonDown)
        l_roll = -dx / 20.0f;
    else
        l_yaw = dx / 20.0f;

    navigate(l_throttle, l_yaw, l_pitch, l_roll);
}

