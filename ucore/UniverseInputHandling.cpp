#include "pch.h"
#include "Universe.h"

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
                space.moveFrame(Movement_Forward, double(-dy * 5.0f));
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

