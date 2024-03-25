#include "pch.h"
#include "Universe.h"

void Universe::ShowDemo(int nParam)
{
    PNT newS;

    switch ((UDemoType)nParam)
    {
    case UDemo_TotalSolarEclipse:

        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(1 * M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        
        SetLockTargetAndMode(&earth, TargetLockMode_ViewTarget);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(-262.135429, 2403.108632, 54.783701);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.2f * M_PI / 5);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);
        Moon_Orbit(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_6p25_Percent);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_TotalSolarEclipseViewFromSun:
        
            // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(1.04 * 3 * M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun

        SetLockTargetAndMode(&earth, TargetLockMode_ViewTarget);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(-68.670399, 1972.622666, 875.368712
        );
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(0.9f * M_PI / 2);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);
        Moon_Orbit(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_6p25_Percent);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_TotalSolarEclipseOnNorthPole:

        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        SetLockTargetAndMode(&earth, TargetLockMode_ViewTarget);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(1044.233749, 2147.958797, 190.950815);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.3f * M_PI / 5);
        Earth_SetOrbitalPositionAngle(0.79f * M_PI / 2);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);
        Moon_Orbit(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_6p25_Percent);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_AnnularSolarEclipseFromSpace:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        ResetFollowTargetAndMode();

        // Set S
        // the hardcoded values here were found by printing the value of S & D on screen using ImGui
        // while manually going to that position.
        newS = PNT(-102.765937, 3070.539521, 59.694453);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, PNT(1220.0681, -2517.4311, -572.8346)),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.15f * M_PI / 5);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);
        Moon_Orbit(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_1p5625_Percent);
        SimulationPause(UCmdParam_Off);

        bSidewaysMotionMode = false;
        break;

    case UDemo_PartialLunarEclipse:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        SetLockTargetAndMode(&earth, TargetLockMode_ViewTarget);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(-452.441935, 1933.795838, 265.271073);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(0.96 * M_PI / 2);
        Earth_SetOrbitalPositionAngle(1.24f * M_PI / 2);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);
        Moon_Orbit(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_6p25_Percent);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_PrecessionMotion:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        // Let the moon hide below the earth's orbital plane so that
        // it won't distract the user
        Moon_SetOrbitalPositionAngle(0);

        // Adjust navigation view locks on earth and sun
        SetLockTargetAndMode(&earth, TargetLockMode_ViewTarget);

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
        Moon_Orbit(UCmdParam_On);

        // Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        bShowPlanetAxis = True;

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_100_Percent);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_SixMonthLongDayAndNightOnNorthPole:

        Moon_SetOrbitalPositionAngle(0);
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        // Set S
        newS = PNT(earth.getCenter().x + 250, earth.getCenter().y - 350, earth.getCenter().z + 300);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        SetLockTargetAndMode(&earth, TargetLockMode_FollowTarget);
        // Adjust navigation view locks on earth and sun

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        //// Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_Off);
        Moon_Orbit(UCmdParam_On);

        //// Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_400_Percent);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle:

        Moon_SetOrbitalPositionAngle(0);
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI + M_PI / 4);

        // Set S
        newS = PNT(earth.getCenter().x, earth.getCenter().y - 350, earth.getCenter().z + 100);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        SetLockTargetAndMode(&earth, TargetLockMode_FollowTarget);
        // Adjust navigation view locks on earth and sun

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        //// Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_Off);
        Moon_Orbit(UCmdParam_On);

        //// Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_400_Percent);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_TiltedOrbitalPlanes:
        Earth_SetOrbitalPositionAngle(M_PI / 2 - M_PI / 6);

        // Tilted orbit demo must show both the orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_On);

        // Adjust navigation view locks on earth and sun
        SetLockTargetAndMode(&earth, TargetLockMode_ViewTarget);

        newS = PNT(691.508170, 5287.321976, 305.415363);
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
        Moon_Orbit(UCmdParam_On);


        SetSimulationSpeed(USimulationSpeed_50_Percent);
        SetDotDensity(UDotDensity_Normal);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_StarParallex:

        Moon_SetOrbitalPositionAngle(0);
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI + M_PI / 4);

        // Set S
        newS = PNT(earth.getCenter().x, earth.getCenter().y - 700, earth.getCenter().z + 200);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        SetLockTargetAndMode(&earth, TargetLockMode_FollowTarget);
        // Adjust navigation view locks on earth and sun

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        //// Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_Off);
        Moon_Orbit(UCmdParam_Off);

        //// Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_1600_Percent);
        SimulationPause(UCmdParam_Off);

        break;
    }

}

