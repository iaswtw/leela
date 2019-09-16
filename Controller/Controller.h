#pragma once

#include "UCore.h"

class Controller
{
public:
    Controller(Universe& core) :
        _core(core),
        space(core.space)
    {
    }

    void ChangeSidewaysMotionMode();
    void SetDefaultView();
    void NavigationLockOntoEarth(int nParam);
    void NavigationLockOntoSun(int nParam);
    void LookAtEarth();
    void LookAtSun();

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
    void Moon_OrbitalPlaneRotation(int nParam);
    void Moon_RevolutionMotion(int nParam);
    void Moon_SetOrbitalPositionAngle(double fAngle);

    void SetDotDensity(int nParam);
    void ShowDemo(int nParam);

    void ChangeBoolean(bool *pBool, int nParam);


public:
    Universe& _core;
    Space& space;

    bool bMouseGrabbed = false;
    bool bFastForward = false;
    bool bFastReverse = false;

    bool bLeftMouseButtonDown = false;
    bool bRightMouseButtonDown = false;
    bool bMiddleMouseButtonDown = false;

    bool bMoveForward = false;
    bool bMoveBackward = false;
    bool bRotateRight = false;
    bool bRotateLeft = false;
    bool bRotateRightSd = false;
    bool bRotateLeftSd = false;


    const float noYaw = 0.0f;
    const float noPitch = 0.0f;
    const float noRoll = 0.0f;
    const float noThrottle = 0.0f;

    const float nominalYaw = 0.5f;
    const float nominalPitch = 0.5f;
    const float nominalRoll = 0.5f;
    const float nominalThrottle = 30.0f;


    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
    float throttle = 0.0f;


    bool bCtrlModifier = false;
    bool bAltModifier = false;
    bool bShiftModifier = false;

    bool bSidewaysMotionMode = true;
    bool bLockOntoEarth = false;
    bool bLockOntoSun = false;

};