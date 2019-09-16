#include "Controller.h"


/*!
****************************************************************************



****************************************************************************/
void Controller::ChangeSidewaysMotionMode()
{
    bSidewaysMotionMode = !bSidewaysMotionMode;
    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Controller::SetDefaultView()
{
    _core.space.pushFrame();

    _core.space.initFrame();
    _core.space.rotateFrame(PNT(0, 0, 0), -10.0, -15.0);
    //space.moveFrame(Movement_Backward, 3400);

    bLockOntoSun = false;
    bLockOntoEarth = false;
    _core.F_REFERENCE_VECTOR_ALONG_Z = 0;

    bUpdateUI = true;
}

/*!
****************************************************************************

 Lock/Unlock/Toggle the earth at the center of the view.  Set the
 reference vector along -Z.

****************************************************************************/
void Controller::NavigationLockOntoEarth(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bLockOntoEarth = true;
        bLockOntoSun = false;
        _core.F_REFERENCE_VECTOR_ALONG_Z = 1;

        ChangeBoolean(&_core.earth.bRevolutionMotion, UCmdParam_Off);
        LookAtEarth();
        break;

    case UCmdParam_Off:
        bLockOntoEarth = false;
        _core.F_REFERENCE_VECTOR_ALONG_Z = 0;
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
void Controller::NavigationLockOntoSun(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bLockOntoSun = true;
        bLockOntoEarth = false;
        _core.F_REFERENCE_VECTOR_ALONG_Z = 1;
        LookAtSun();
        break;

    case UCmdParam_Off:
        bLockOntoSun = false;
        _core.F_REFERENCE_VECTOR_ALONG_Z = 0;
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

void Controller::LookAtEarth()
{
    _core.space.setFrame(AT_POINT,
        space.S,
        VECTOR(space.S, _core.earth.getCenter()),
        PNT(space.S.x, space.S.y, space.S.z - 100));

}

void Controller::LookAtSun()
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
void Controller::SetSimulationSpeed(int nParam)
{
    eSimulationSpeed = USimulationSpeedType(nParam);
    switch (eSimulationSpeed)
    {
    case USimulationSpeed_VeryLow:
        _core._stepMultiplier = 0.005;
        break;

    case USimulationSpeed_Low2:
        _stepMultiplier = 0.015;
        break;

    case USimulationSpeed_Low1:
        _stepMultiplier = 0.03;
        break;

    case USimulationSpeed_Low:
        _stepMultiplier = 0.05;
        break;

    case USimulationSpeed_Normal:
        _stepMultiplier = 0.36;
        break;

    case USimulationSpeed_High:
        _stepMultiplier = 3;
        break;

    case USimulationSpeed_VeryHigh:
        _stepMultiplier = 10;
        break;
    }

    bUpdateUI = true;
}

/*!
****************************************************************************

 Set time flow direction to either Forward or Reverse depending on the
 argument.

****************************************************************************/
void Controller::SetTimeDirection(int nParam)
{
    eTimeDirection = (UTimeDirectionType)nParam;
    bUpdateUI = true;
}

/*!
****************************************************************************

 Turn On/Off Fast forward motion.

****************************************************************************/
void Controller::FastForward(int nParam)
{
    ChangeBoolean(&bFastForward, nParam);
}

/*!
****************************************************************************

 Turn On/Off fast Reverse motion.

****************************************************************************/
void Controller::Rewind(int nParam)
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
void Controller::ChangeBoolean(bool *pBool, int nParam)
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
void Controller::SimulationPause(int nParam)
{
    ChangeBoolean(&bSimulationPause, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Controller::Earth_RotationMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Controller::Earth_RevolutionMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    //F_REFERENCE_VECTOR_ALONG_Z = 0;
    //bLockOntoEarth = false;

    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Controller::Earth_PrecessionMotion(int nParam)
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
void Controller::Earth_OrbitalPlane(int nParam)
{
    ChangeBoolean(&earthRenderer.bShowOrbitalPlane, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Controller::Moon_OrbitalPlane(int nParam)
{
    ChangeBoolean(&moonRenderer.bShowOrbitalPlane, nParam);
    bUpdateUI = true;

}


/*!
****************************************************************************



****************************************************************************/
void Controller::Moon_OrbitalPlaneRotation(int nParam)
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
void Controller::Moon_RevolutionMotion(int nParam)
{
    ChangeBoolean(&moon.bRevolutionMotion, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Controller::SetDotDensity(int nParam)
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
void Controller::ShowDemo(int nParam)
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

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(-323.5104, 957.3688, 296.3317);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.2 * M_PI / 5);

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

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(733.4838, 817.9659, 297.3985);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.7 * M_PI / 5);
        Earth_SetOrbitalPositionAngle(0.69 * M_PI / 2);

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

        // Set S
        // the hardcoded values here were found by printing the value of S & D on screen using ImGui
        // while manually going to that position.
        newS = PNT(-92.2673, 1435.5368, 52.9889);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, PNT(641.3813, -2678.9711, -409.2932)),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.3 * M_PI / 5);

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

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(1196.26, 462.93, -60.55);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(0.35 * M_PI / 2);
        Earth_SetOrbitalPositionAngle(0.59 * M_PI / 2);

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

    case UDemo_TiltedOrbitalPlanes:
        Earth_SetOrbitalPositionAngle(M_PI / 2 - M_PI / 10);

        // Tilted orbit demo must show both the orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_On);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_Off);
        NavigationLockOntoSun(UCmdParam_Off);

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


/*!
****************************************************************************



****************************************************************************/
void Controller::Moon_SetOrbitalPositionAngle(double fAngle)
{
    moon.setOrbitalAngle(fAngle);
    moon.calculateCenterPosition();
}

/*!
****************************************************************************



****************************************************************************/
void Controller::Earth_SetOrbitalPositionAngle(double fAngle)
{
    earth.setOrbitalAngle(fAngle);
    earth.calculateCenterPosition();
}



/*************************************************************************************************


**************************************************************************************************/
void Controller::processFlags()
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

}


