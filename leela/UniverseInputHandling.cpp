#include "Universe.h"
#include <spdlog/spdlog.h>

constexpr int LOG_BUFFER_SIZE = 1000;
static char logBuffer[LOG_BUFFER_SIZE];

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
        break;
    case SDLK_c:
        ToggleFollowTarget(sun, TargetLockMode_ViewTarget);
        break;
    case SDLK_d:
        //if (isAllModifiers()) {
        //    std::snprintf(logBuffer, LOG_BUFFER_SIZE, "%f, %f, %f", space.D.x, space.D.y, space.D.z);
        //    ImGui::LogToClipboard();
        //    ImGui::LogText(logBuffer);
        //    ImGui::LogFinish();
        //    spdlog::info("D = {}", logBuffer);
        //}
        //else
            SetDefaultView();
        break;
    case SDLK_e:
        //if (isAllModifiers()) {
        //    std::snprintf(logBuffer, LOG_BUFFER_SIZE, "Earth _orbitalAngle = %f", earth._orbitalAngle);
        //    logString = logBuffer;                  // ImGui will check the logString (of type std::string), and write its contents to Windows clipboard.
        //    spdlog::info(logBuffer);
        //}
        //else
            Earth_OrbitalPlane(UCmdParam_Toggle);
        break;
    case SDLK_f:
        bEquals = true;
        break;
    case SDLK_l:
        cycleLockMode();
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
        bMinus = true;
        break;
    case SDLK_s:
        if (isAllModifiers())
        {
            std::snprintf(
                logBuffer,
                LOG_BUFFER_SIZE,
                "\nS = %f, %f, %f"
                "\nD = %f, %f, %f"
                "\nEarth->_orbitalAngle = %f"
                "\nmoon->_nodalPrecessionAngle = %f"
                ,
                space.S.x, space.S.y, space.S.z,
                space.D.x, space.D.y, space.D.z,
                earth->_orbitalAngle,
                moon->_nodalPrecessionAngle
            );
            logString = logBuffer;                  // ImGui will check the logString (of type std::string), and write its contents to Windows clipboard.
            spdlog::info(logBuffer);
        }
        // TODO GL_INVALID_OPERATION if stars are disabled
        //else
        //{
        //    ChangeBoolean(&bGalaxyStars, UCmdParam_Toggle);
        //}
        break;
    case SDLK_v:
        ChangeSidewaysMotionMode();
        break;
    case SDLK_x:
        ToggleFollowTarget(moon, TargetLockMode_ViewTarget);
        break;
    case SDLK_z:
        if (isNoModifier())
            ToggleFollowTarget(earth, TargetLockMode_ViewTarget);
        else
            if (bAltModifier)
                space.setFrameMoveMode(SHORT_D_MODE);
        break;

    // ============= Digits ===============
    case SDLK_0:
        Earth_RevolutionMotion(UCmdParam_Toggle);
        break;

    case SDLK_1:
        // TODO  Pressing Ctrl, then 1 does not cause this even to be raised.
        //spdlog::info("1 down");
        bEquals = true;
        break;


    case SDLK_4:
        Earth_SetOrbitalPositionAngle(0);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_5:
        Earth_SetOrbitalPositionAngle(M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_6:
        Earth_SetOrbitalPositionAngle(M_PI);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_7:
        Earth_SetOrbitalPositionAngle(3 * M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_9:
        Moon_RevolutionMotion(UCmdParam_Toggle);
        break;


    // ============ Function keys =============
    case SDLK_F3:
        bRetardEarthInOrbit = true;
        break;
    case SDLK_F4:
        bAdvanceEarthInOrbit = true;
        break;
    case SDLK_F5:
        if (bShiftModifier)
            Moon_OrbitalPlaneRotation(UCmdParam_Reset);
        else
            Moon_OrbitalPlaneRotation(UCmdParam_Toggle);
        break;
    case SDLK_F6:
        if (bShiftModifier)
            Earth_PrecessionMotion(UCmdParam_Reset);
        else
            Earth_PrecessionMotion(UCmdParam_Toggle);
        break;

    case SDLK_F7:
        bRetardMoonInOrbit = true;
        break;
    case SDLK_F8:
        bAdvanceMoonInOrbit = true;
        break;


    case SDLK_F9:
        toggleControlPanelVisibilityWhenMouseGrabbed();
        break;
    case SDLK_F11:
        toggleFullScreen();
        break;



    case SDLK_BACKQUOTE:
        bMinus = true;
        break;


        // Arrow keys
    case SDLK_LEFT:
        DecreaseSimulationSpeed();
        break;
    case SDLK_RIGHT:
        IncreaseSimulationSpeed();
        break;

    case SDLK_MINUS:
        bMinus = true;
        break;
    case SDLK_EQUALS:
        bEquals = true;
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
        bEquals = false;
        break;
    case SDLK_r:
        bMinus = false;
        break;



    // ============= Digits ===============
    case SDLK_1:
        bEquals = false;
        break;


    // ============ Function keys =============
    case SDLK_F3:
        bRetardEarthInOrbit = false;
        break;
    case SDLK_F4:
        bAdvanceEarthInOrbit = false;
        break;
    case SDLK_F7:
        bRetardMoonInOrbit = false;
        break;
    case SDLK_F8:
        bAdvanceMoonInOrbit = false;
        break;


    case SDLK_BACKQUOTE:
        bMinus = false;
        break;


    case SDLK_MINUS:
        bMinus = false;
        break;
    case SDLK_EQUALS:
        bEquals = false;
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

// return true if no modifier is set.
bool Universe::isNoModifier()
{
    if (!bShiftModifier && !bCtrlModifier && !bAltModifier)
        return true;
    else
        return false;
}

// return true if all modifiers are set.
bool Universe::isAllModifiers()
{
    if (bShiftModifier && bCtrlModifier && bAltModifier)
        return true;
    else
        return false;
}