#include "Universe.h"

void Universe::onKeyDown(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_a:
        if (isNoModifier())
            ChangeBoolean(&bShowAxis, UCmdParam_Toggle);
        else
            if (bAltModifier)
                space.setFrameMoveMode(MEDIUM_D_MODE);
        break;
    case SDLK_b:
        ToggleConstantDirectionFollowMode();
        break;
    case SDLK_c:
        ToggleFollowTarget(&sun, TargetLockMode_ViewTarget);
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
    case SDLK_l:
        ToggleFollowTarget(&earth, TargetLockMode_ViewTarget);
        break;
    case SDLK_n:
        bEarthSurfaceLockMode = !bEarthSurfaceLockMode;
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
    case SDLK_q:
        if (bAltModifier)
            space.setFrameMoveMode(D_MODE);
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
        if (bAltModifier)
            space.setFrameMoveMode(SHORT_D_MODE);
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
        keyboard_throttle = nominalThrottle;
        break;
    case SDLK_END:
        keyboard_throttle = -nominalThrottle;
        break;
    case SDLK_PAGEDOWN:
        keyboard_yaw = nominalYaw;
        break;
    case SDLK_DELETE:
        keyboard_yaw = -nominalYaw;
        break;
    case SDLK_PAGEUP:
        if (!bAltModifier)
            keyboard_pitch = nominalPitch;
        else
            keyboard_roll = -nominalRoll;
        break;
    case SDLK_INSERT:
        if (!bAltModifier)
            keyboard_pitch = -nominalPitch;
        else
            keyboard_roll = nominalRoll;
        break;

        // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        bCtrlModifier = true;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        bShiftModifier = true;
        bSidewaysMotionMode = true;
        //clearAllFirFilters();

        break;
    case SDLK_LALT:
    case SDLK_RALT:
        bAltModifier = true;
        space.setFrameMoveMode(D_MODE);
        //clearAllFirFilters();
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
        keyboard_throttle = noThrottle;
        break;
    case SDLK_PAGEDOWN:
    case SDLK_DELETE:
        keyboard_yaw = noYaw;
        break;
    case SDLK_PAGEUP:
    case SDLK_INSERT:
        keyboard_roll = noRoll;
        keyboard_pitch = noPitch;
        break;


        // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        bCtrlModifier = false;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        bShiftModifier = false;
        bSidewaysMotionMode = false;
        //clearAllFirFilters();
        break;
    case SDLK_LALT:
    case SDLK_RALT:
        bAltModifier = false;
        space.setFrameMoveMode(S_MODE);
        //clearAllFirFilters();
        break;

    }

}

void Universe::onMouseMotion(int xrel, int yrel)
{
    mouse_dx = float(xrel);
    mouse_dy = float(yrel);
}

bool Universe::isNoModifier()
{
    if (!bShiftModifier && !bCtrlModifier && !bAltModifier)
        return true;
    else
        return false;
}