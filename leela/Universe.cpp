#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include "Universe.h"
#include "Utils.h"
#include <stdio.h>
#include <string>

#include <spdlog/spdlog.h>

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

    sunRenderer(*this, sun, "sunmap.png"),
    earthRenderer(*this, earth, "earth-erde-mit-wolken-2k.png"),
    moonRenderer(*this, moon, "moonmap1k.png"),
    marsRenderer(*this, mars, "mars2k.png"),
    jupiterRenderer(*this, jupiter, "jupiter2k.png"),
    saturnRenderer(*this, saturn, "saturn2k.png"),
    uranusRenderer(*this, uranus, "uranus2k.png"),
    neptuneRenderer(*this, neptune, "neptune2k.png"),

    starsRenderer(stars),

    throttleFilter                  (FIR_WIDTH, fir_coeff),
    yawFilter                       (FIR_WIDTH, fir_coeff),
    pitchFilter                     (FIR_WIDTH, fir_coeff),
    rollFilter                      (FIR_WIDTH, fir_coeff),
    stepMultiplierFilterWhenPaused  (FIR_WIDTH, fir_coeff),
    stepMultiplierFilter            (FIR_WIDTH, fir_coeff),

    monthLabelsRenderer(earth),

    bm1(&earth),
    bm2(&earth),
    bm3(&earth)
    
{
}


Universe::~Universe()
{
}

void Universe::compileShaders()
{
    spdlog::info("Compiling all GLSL programs");

    planetGlslProgram.compileShadersFromFile("planet.vert.glsl", "planet.frag.glsl");
    planetGlslProgram.link();

    sunGlslProgram.compileShadersFromFile("sun.vert.glsl", "sun.frag.glsl");
    sunGlslProgram.link();

    starGlslProgram.compileShadersFromFile("star.vert.glsl", "star.frag.glsl");
    starGlslProgram.link();

    simpleGlslProgram.compileShadersFromFile("simple.vert.glsl", "simple.frag.glsl");
    simpleGlslProgram.link();

    fontGlslProgram.compileShadersFromFile("font.vert.glsl", "font.frag.glsl");
    fontGlslProgram.link();

    bookmarkGlslProgram.compileShadersFromFile("bookmark.vert.glsl", "bookmark.frag.glsl");
    bookmarkGlslProgram.link();
}

/*************************************************************************************************
 Initialize various parameters of planets, stars, etc.  Parameters include radius of objects,
 time periods of rotation and revolution, colors, etc.
**************************************************************************************************/
void Universe::initSceneObjectsAndComponents()
{
    stars.setCubeStarParameters(
        3500,                               // total number of stars (single + double pixel)
        0.6f                                // radio of single pixel starts to all stars
    );


    coordinateAxisRenderer.setSpan(3600, 3600, 1200);
    coordinateAxisRenderer.setColors(
        glm::vec3(0.2f, 0.2f, 0.5f),        // X axis color
        glm::vec3(0.2f, 0.5f, 0.2f),        // Y axis color
        glm::vec3(0.2f, 0.5f, 0.5f)         // Z axis color
    );
    scene.addComponent(&coordinateAxisRenderer);

    // Sun
    //---------------------------------------
    //sun.setColor(0.7f, 0.7f, 0.1f);
    sun.setColor(1.0f, 1.0f, 0.6f);
    sun.setRotationParameters(160,          // radius
        0,                                  // initial rotation angle
        0.002f,                              // rotation velocity
        glm::radians(0.0f),                 // axis tilt orientation angle
        glm::radians(0.0f)                  // axis tilt angle
    );
    sun.setOrbitalParameters(0,             // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                              // revolution velocity
        0,                                  // nodal precession initial angle
        0                                   // orbital tilt
    );
    scene.addSceneObject(&sun);

    // Earth
    //---------------------------------------
    earth.setName("Earth");
    earth.setColor(0.55f, 0.82f, 1.0f);
    earth.setRotationParameters(80,         // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(0.0f),                 // axis tilt orientation angle
        glm::radians(23.5f)                 // axis tilt angle
    );
    earth.setOrbitalParameters(3000,        // radius of orbit
        glm::radians(30.0f),                 // initial orbital angle
        0.001f,                             // revolution velocity
        0.0f,                               // nodal precession initial angle
        0                                   // orbital  tilt
    );
    earth.setOrbitalPlaneColor(glm::vec3(0.55, 0.82, 1.0));
    earth.setSunSphere(&sun);
    sun.addSceneObject(&earth);

    // Moon
    //---------------------------------------
    moon.setName("Moon");
    moon.setColor(0.8f, 0.8f, 0.8f);
    moon.setRotationParameters(22,          // radius
        0,                                  // initial rotation angle
        0.008888888f,                       // rotation velocity
        glm::radians(0.0f),                 // axis tilt orientation angle
        glm::radians(10.0f)                 // axis tilt angle
    );
    moon.setOrbitalParameters(400,          // radius of orbit
        0.0f,                               // initial orbital angle
        0.008888888f,                       // revolution velocity
        0,                                  // nodal precession initial angle
        glm::radians(30.0f)                 // orbital tilt
    );
    moon.setOrbitalPlaneColor(glm::vec3(0.8f, 0.8f, 0.8f));
    moon.setSunSphere(&sun);
    moon.setRelatedSphere(&earth);
    earth.setRelatedSphere(&moon);
    earth.addSceneObject(&moon);

    // Mars
    //---------------------------------------
    mars.setName("Mars");
    mars.setColor(0.85f, 0.5f, 0.5f);
    mars.setRotationParameters(60,          // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(270.0f),               // axis tilt orientation angle
        glm::radians(25.0f)                 // axis tilt angle
    );
    mars.setOrbitalParameters(7000,         // radius of orbit
        0,                                  // initial orbital angle
        0.0006f,                            // revolution velocity
        0.0f,                               // nodal precession initial angle
        glm::radians(1.85f)                 // orbital tilt
    );
    mars.setOrbitalPlaneColor(glm::vec3(0.85, 0.5, 0.5f));
    mars.setSunSphere(&sun);
    sun.addSceneObject(&mars);

    // Jupiter
    //---------------------------------------
    jupiter.setName("Jupiter");
    jupiter.setColor(0.85f, 0.7f, 0.6f);
    jupiter.setRotationParameters(140,      // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(270.0f),               // axis tilt orientation angle
        glm::radians(3.1f)                  // axis tilt angle
    );
    jupiter.setOrbitalParameters(11000,      // radius of orbit
        glm::radians(280.0f),               // initial orbital angle
        0.0004f,                            // revolution velocity
        0.0f,                               // nodal precession initial angle
        glm::radians(1.31f)                 // orbital tilt
    );
    jupiter.setOrbitalPlaneColor(glm::vec3(0.85, 0.5, 0.5f));
    jupiter.setSunSphere(&sun);
    sun.addSceneObject(&jupiter);


    // Saturn
    //---------------------------------------
    saturn.setName("Saturn");
    saturn.setColor(0.7f, 0.7f, 0.4f);
    saturn.setRotationParameters(110,       // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(270.0f),               // axis tilt orientation angle
        glm::radians(26.7f)                 // axis tilt angle
    );
    saturn.setOrbitalParameters(15000,       // radius of orbit
        glm::radians(220.0f),               // initial orbital angle
        0.0002f,                            // revolution velocity
        0.0f,                               // nodal precession initial angle
        glm::radians(2.49f)                 // orbital tilt
    );
    saturn.setOrbitalPlaneColor(glm::vec3(0.85, 0.5, 0.5f));
    saturn.setSunSphere(&sun);
    sun.addSceneObject(&saturn);


    // Uranus
    //---------------------------------------
    uranus.setName("Uranus");
    uranus.setColor(0.7f, 0.7f, 0.85f);
    uranus.setRotationParameters(90,       // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(270.0f),               // axis tilt orientation angle
        glm::radians(97.7f)                 // axis tilt angle
    );
    uranus.setOrbitalParameters(20000,      // radius of orbit
        glm::radians(18.0f),                // initial orbital angle
        0.0001f,                            // revolution velocity
        0.0f,                               // nodal precession initial angle
        glm::radians(.77f)                  // orbital tilt
    );
    uranus.setOrbitalPlaneColor(glm::vec3(0.85, 0.5, 0.5f));
    uranus.setSunSphere(&sun);
    sun.addSceneObject(&uranus);


    // Neptune
    //---------------------------------------
    neptune.setName("Neptune");
    neptune.setColor(0.4f, 0.4f, 0.9f);
    neptune.setRotationParameters(80,        // radius
        0,                                   // initial rotation angle
        0.02f,                               // rotation velocity
        glm::radians(270.0f),                // axis tilt orientation angle
        glm::radians(28.0f)                  // axis tilt angle
    );
    neptune.setOrbitalParameters(25000,      // radius of orbit
        glm::radians(150.0f),                // initial orbital angle
        0.00008f,                            // revolution velocity
        0.0f,                                // nodal precession initial angle
        glm::radians(1.77f)                  // orbital tilt
    );
    neptune.setOrbitalPlaneColor(glm::vec3(0.85, 0.5, 0.5f));
    neptune.setSunSphere(&sun);
    sun.addSceneObject(&neptune);


    //---------------------------------------------------------------
    SetApplicationStartView();


    //=================================================================

    coordinateAxisRenderer.init();

    sunRenderer.setAsLightSource();
    sunRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    sun.addComponent(&sunRenderer);
    sunRenderer.init();

    earthRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
    earthRenderer.constructVerticesAndSendToGpu();
    earthRenderer.bShowLatitudesAndLongitudes = true;
    earthRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    earthRenderer.init();
    earth.addComponent(&earthRenderer);

    moonRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
    moonRenderer.constructVerticesAndSendToGpu();
    moonRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    moonRenderer.init();
    moon.addComponent(&moonRenderer);

    marsRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    marsRenderer.constructVerticesAndSendToGpu();
    marsRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    marsRenderer.bShowOrbit = False;
    marsRenderer.init();
    mars.addComponent(&marsRenderer);

    jupiterRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    jupiterRenderer.constructVerticesAndSendToGpu();
    jupiterRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    jupiterRenderer.bShowOrbit = False;
    jupiterRenderer.init();
    jupiter.addComponent(&jupiterRenderer);

    saturnRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    saturnRenderer.constructVerticesAndSendToGpu();
    saturnRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    saturnRenderer.bShowOrbit = False;
    saturnRenderer.init();
    saturn.addComponent(&saturnRenderer);

    uranusRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    uranusRenderer.constructVerticesAndSendToGpu();
    uranusRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    uranusRenderer.bShowOrbit = False;
    uranusRenderer.init();
    uranus.addComponent(&uranusRenderer);

    neptuneRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    neptuneRenderer.constructVerticesAndSendToGpu();
    neptuneRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    neptuneRenderer.bShowOrbit = False;
    neptuneRenderer.init();
    neptune.addComponent(&neptuneRenderer);

    //------------------------------

    constructFontInfrastructureAndSendToGpu();


    monthLabelsRenderer.init();

    bm1.init();
    bm2.init();
    bm3.init();

    bm1.addComponent(&bmr1);
    bm2.addComponent(&bmr2);
    bm3.addComponent(&bmr3);

    bm1.set("Austin", 30.2f, -97.7);
    bm2.set("Chicago", 41.8f, -87.6f);
    bm3.set("Cheyenne", 41.1f, -104.8);


    earth.addComponent(&monthLabelsRenderer);
    earth.addSceneObject(&bm1);
    earth.addSceneObject(&bm2);
    earth.addSceneObject(&bm3);

    bmr1.init();
    bmr2.init();
    bmr3.init();

    
    //---------------------------------------------------------------------------------------------------
    
    scene.addComponent(&starsRenderer);
    starsRenderer.init();

    SphericalBodyRenderer(*this, earth);

    SceneObject::printTree(&scene);

    glBindVertexArray(0);       // Disable VBO

}

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

void Universe::_advanceSceneObject(SceneObject* sceneObject, float stepMultiplier)
{
    //----------------------------------------
    // todo - eventually implement this without recursion
    //----------------------------------------

    // Advance just the given scene object
    sceneObject->advance(stepMultiplier);

    // advance all compnents of this scene object
    for (Component *c : sceneObject->_components)
    {
        c->advance(stepMultiplier);
    }

    // finally, advance each child scene object (recursively)
    //  - advancing `this` (i.e. given scene object) first ensures children are advanced after parent.
    for (SceneObject* obj : sceneObject->_childSceneObjects)
    {
        _advanceSceneObject(obj, stepMultiplier);
    }

}

void Universe::advanceScene(float stepMultiplier)
{
    _advanceSceneObject(&scene, stepMultiplier);
}


void Universe::ChangeSidewaysMotionMode()
{
    bSidewaysMotionMode = !bSidewaysMotionMode;
}

void Universe::SetDefaultView()
{
    space.pushFrame();

    space.initFrame();
    space.rotateFrame(PNT(0, 0, 0), 0, -15.0);
    //space.rotateFrame(PNT(0, 0, 0), 0, 0);

    space.moveFrame(Movement_Backward, 7000.0f);

    ResetFollowTargetAndMode();

}

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


void Universe::SetLockTargetAndMode(SphericalBody* target, TargetLockMode mode)
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
        downDirection = VECTOR(space.D, space.R);

        if ((mode == TargetLockMode_FollowTarget) || (mode == TargetLockMode_OrientedViewTarget))
        {
            calculateCommonTargetLockVariables();

            bSidewaysMotionMode = false;

            if (mode == TargetLockMode_FollowTarget)
            {
                calculateFollowTargetLockVariables();
            }
            else if (mode == TargetLockMode_OrientedViewTarget)
            {
                calculateOrientedViewLockVariables();
            }

        }
    }
}

void Universe::calculateCommonTargetLockVariables()
{
    PNT T = lockTarget->getCenter();
    PNT S = space.S;
    followDistance = float(S.distanceTo(T));

    spdlog::info("followDistance = {}", followDistance);

}

void Universe::calculateFollowTargetLockVariables()
{
    PNT T = lockTarget->getCenter();
    PNT S = space.S;
    followVector = VECTOR(S, T);
}

//
// Calculate `orientedTargetLock_alpha` and `orientedTargetLock_beta` members.
//
void Universe::calculateOrientedViewLockVariables()
{
    // T = target's center
    // P = target's parent's center
    // S = observer's position
    // 
    // Goal is to calculate 2 things:
    //   1. observer's angle from the normal of the orbital plane of the target sphere.
    //   2. angle of the vector (to be used as axis of rotation for #1) from target's orbital radius vector
    //        - this vector that lies in the target's orbital plane
    //

    orientedTargetLock_alpha = 0.0f;
    orientedTargetLock_beta = 0.0f;
    if (lockTarget->_sphericalBodyParent)
    {
        PNT T = lockTarget->getCenter();
        PNT S = space.S;
        PNT P = PNT(lockTarget->_sphericalBodyParent->getCenter());
        VECTOR normal = VECTOR(lockTarget->_orbitalNormal);
        VECTOR PT = VECTOR(P, T);               // e.g. if target is earth, this is a vector from center of sun to center of earth.
        VECTOR TS = VECTOR(T, S);               // S is where we are

        VECTOR axis = space.crossProduct(TS, normal);         // this is NOT the target's rotation axis!

        //VECTOR axis = normal.cross(TS);         // this is NOT the target's rotation axis!
                                                // This is the axis aroud which a `translated` T will have to be rotated to get the observer's position S for the next frame.

        //spdlog::info("--------------------------------");
        //spdlog::info("calculateOrientedViewLockVariables:");
        //spdlog::info("--------------------------------");

        orientedTargetLock_alpha = axis.angleFrom(PT, normal.cross(PT));
        //spdlog::info("   orientedTargetLock_alpha = {}", orientedTargetLock_alpha * 180 / M_PI);

        orientedTargetLock_beta = normal.angleFrom(TS, normal);                 // reference doesn't matter since it is from normal.
        //spdlog::info("   orientedTargetLock_beta = {}", orientedTargetLock_beta * 180 / M_PI);

        //spdlog::info("   S  = {}, {}, {}", S.x, S.y, S.z);
        //spdlog::info("   PT = {}, {}, {}", PT.x, PT.y, PT.z);
        //spdlog::info("   TS = {}, {}, {}", TS.x, TS.y, TS.z);
        //spdlog::info("   normal = {}, {}, {}", lockTarget->_orbitalNormal.x, lockTarget->_orbitalNormal.y, lockTarget->_orbitalNormal.z);
        //spdlog::info("   normal x TS = {}, {}, {}", axis.x, axis.y, axis.z);
        //spdlog::info("--------------------------------");
        //spdlog::info("");
    }
}

void Universe::cycleLockMode()
{
    if (lockMode == TargetLockMode_ViewTarget)
        SetLockMode(TargetLockMode_FollowTarget);
    else if (lockMode == TargetLockMode_FollowTarget)
        SetLockMode(TargetLockMode_OrientedViewTarget);
    else
        SetLockMode(TargetLockMode_ViewTarget);
}

void Universe::ResetFollowTargetAndMode()
{
    SetLockTargetAndMode(nullptr, TargetLockMode_ViewTarget);
}

void Universe::ToggleFollowTarget(SphericalBody* target, TargetLockMode mode)
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
    else if (lockMode == TargetLockMode_OrientedViewTarget)
    {
        PNT T = lockTarget->getCenter();

        // TODO _sphericalBodyParent may be null
        PNT P = PNT(lockTarget->_sphericalBodyParent->getCenter());

        VECTOR PT = VECTOR(P, T);               // e.g. if target is earth, this is a vector from center of sun to center of earth.
        VECTOR normal = VECTOR(lockTarget->_orbitalNormal);

        PNT A = T.translated(100, PT);
        PNT N = T.translated(100, normal);
        PNT rotatedA = space.rotate(T, N, A, space.deg(orientedTargetLock_alpha));

        PNT S = T.translated(followDistance, normal);
        PNT newS = space.rotate(rotatedA, T, S, space.deg(orientedTargetLock_beta));

        //spdlog::info("Applying followDistance = {}", followDistance);
        //spdlog::info("orientedTargetLock_alpha = {}", orientedTargetLock_alpha * 180 / M_PI);
        //spdlog::info("orientedTargetLock_beta = {}", orientedTargetLock_beta * 180 / M_PI);
        //spdlog::info("A = {}, {}, {}", A.x, A.y, A.z);
        //spdlog::info("rotatedA = {}, {}, {}", rotatedA.x, rotatedA.y, rotatedA.z);
        //spdlog::info("newS = {}, {}, {}", newS.x, newS.y, newS.z);
        //spdlog::info("");

        space.setFrame(
            AT_POINT,
            newS,
            VECTOR(newS, lockTarget->getCenter()),
            //space.S.translated(100, downDirection));
            //PNT(space.R.x, space.R.y, space.R.z));
            PNT(newS.x, newS.y, newS.z - 100));
    }
    else
    {
        // TargetLockMode_ViewTarget
        space.setFrame(
            AT_POINT,
            space.S,
            VECTOR(space.S, lockTarget->getCenter()),
            //space.S.translated(100, downDirection));
            //space.S.translated(100, VECTOR(space.D, space.R)));
            //PNT(space.R.x, space.R.y, space.R.z));
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
    case USimulationSpeed_1p5625_Percent:      _stepMultiplier = float(1.0 / pow(2, 6));        break;
    case USimulationSpeed_3p125_Percent:       _stepMultiplier = float(1.0 / pow(2, 5));        break;
    case USimulationSpeed_6p25_Percent:        _stepMultiplier = float(1.0 / pow(2, 4));        break;
    case USimulationSpeed_12p5_Percent:        _stepMultiplier = float(1.0 / pow(2, 3));        break;
    case USimulationSpeed_25_Percent:          _stepMultiplier = float(1.0 / pow(2, 2));        break;
    case USimulationSpeed_50_Percent:          _stepMultiplier = float(1.0 / pow(2, 1));        break;
    case USimulationSpeed_100_Percent:         _stepMultiplier = float(1.0 / pow(2, 0));        break;
    case USimulationSpeed_400_Percent:         _stepMultiplier = float(1.0 / pow(2, -2));        break;
    case USimulationSpeed_1600_Percent:        _stepMultiplier = float(1.0 / pow(2, -4));        break;
    }
}

/*
 * Set time flow direction to either Forward or Reverse depending on the
 * argument.
*/
void Universe::SetTimeDirection(int nParam)
{
    eTimeDirection = (UTimeDirectionType)nParam;
}

// Turn On/Off Fast forward motion.
void Universe::FastForward(int nParam)
{
    ChangeBoolean(&bEquals, nParam);
}

// Turn On/Off fast Reverse motion.
void Universe::Rewind(int nParam)
{
    ChangeBoolean(&bMinus, nParam);
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

void Universe::SimulationPause(int nParam)
{
    ChangeBoolean(&bSimulationPause, nParam);
}

void Universe::Earth_RotationMotion(int nParam)
{
    ChangeBoolean(&earth.bRotationMotion, nParam);
}

void Universe::Earth_RevolutionMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    //F_REFERENCE_VECTOR_ALONG_Z = 0;
    //bLockOntoEarth = false;
}


void Universe::Earth_PrecessionMotion(int nParam)
{
    if (nParam == UCmdParam_Reset)
    {
        earth.bPrecessionMotion = false;
        earth._axisTiltOrientationAngle = glm::radians(0.0f);
    }
    else
    {
        ChangeBoolean(&earth.bPrecessionMotion, nParam);
    }
}

void Universe::Earth_OrbitalPlane(int nParam)
{
    ChangeBoolean(&earthRenderer.bShowOrbitalPlane, nParam);
}

void Universe::Earth_Orbit(int nParam)
{
    ChangeBoolean(&earthRenderer.bShowOrbit, nParam);
}

void Universe::Moon_OrbitalPlane(int nParam)
{
    ChangeBoolean(&moonRenderer.bShowOrbitalPlane, nParam);
}


void Universe::Moon_Orbit(int nParam)
{
    ChangeBoolean(&moonRenderer.bShowOrbit, nParam);
}

void Universe::Moon_OrbitalPlaneRotation(int nParam)
{

    if (nParam == UCmdParam_Reset)
    {
        moon.bOrbitalPlaneRotation = false;
        moon._nodalPrecessionAngle = glm::radians(0.0f);
    }
    else
    {
        ChangeBoolean(&moon.bOrbitalPlaneRotation, nParam);
    }
}

void Universe::Moon_RevolutionMotion(int nParam)
{
    ChangeBoolean(&moon.bRevolutionMotion, nParam);
}

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

void Universe::toggleControlPanelVisibilityWhenMouseGrabbed()
{
    bAlwaysShowControlPanel = !bAlwaysShowControlPanel;
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

void Universe::Moon_SetOrbitalPositionAngle(double fAngle)
{
    moon.setOrbitalAngle(float(fAngle));
    moon.calculateCenterPosition();
}

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


void Universe::processFlags()
{
    //----------------------------------------------
    // First half of this method applies FIR filtering to various motion inputs.
    //----------------------------------------------

    static const float EPSILON = 0.00001f;
    static const float alpha = 0.05f;

    // low pass filtered inputs in various directions.
    float throttle = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
    float step_multiplier_input = 0.0f;


    // adjust gain w.r.t. current frame rate. Slower framerate result in higher gain to keep real-time behaviour same.
    float gain = 1.0f;
    float frameRate = ImGui::GetIO().Framerate;
    if (frameRate > 5)
        gain = REFERENCE_FRAME_RATE / frameRate;
    else
        gain = REFERENCE_FRAME_RATE / 5.0f;

    //-------------------------------------
    // initialize motions due to mouse to zero.
    //-------------------------------------
    mouse_throttle    = 0.0f;
    mouse_yaw         = 0.0f;
    mouse_pitch       = 0.0f;
    mouse_roll        = 0.0f;

    //-------------------------------------
    // calculate motions due to mouse horizontal and vertical motion.
    //-------------------------------------
    if (bLeftMouseButtonDown)
        mouse_throttle = -mouse_dy * 5.0f;
    else
        mouse_pitch = -mouse_dy / 20.0f;

    if (bRightMouseButtonDown)
        mouse_roll = -mouse_dx / 20.0f;
    else
        mouse_yaw = mouse_dx / 20.0f;


    //-------------------------------------
    // Combine mouse and keyboard inputs.
    //-------------------------------------
    throttle  = mouse_throttle + keyboard_throttle + new_mouse_wheel_throttle;
    yaw       = mouse_yaw      + keyboard_yaw;
    pitch     = mouse_pitch    + keyboard_pitch;
    roll      = mouse_roll     + keyboard_roll;

    //-------------------------------------
    // Compensate for low/high framerate.
    // TODO - Not sure if this is the right way to do it.
    //-------------------------------------
    gain /= 40;

    throttle    *= gain;
    yaw         *= gain;
    pitch       *= gain;
    roll        *= gain;


    // Amplify or attenuate the value based on keyboard modifiers.
    if (bCtrlModifier)
        if (bAltModifier)
            throttle /= 100;             // ctrl + alt = super slow zoom
        else
            throttle /= 10;              // ctrl       = slow zoom
    else if (bShiftModifier)
        if (bAltModifier)
            throttle *= 100;             // shift + alt = super fast zoom
        else
            throttle *= 10;              // shift       = fast zoom

    if (bCtrlModifier)
        yaw /= 10;
    else if (bShiftModifier)
        yaw *= 10;

    if (bCtrlModifier)
        pitch /= 100;
    else if (bShiftModifier)
        pitch *= 10;

    if (bCtrlModifier)
        roll /= 10;
    else if (bShiftModifier)
        roll *= 100;


    //-------------------------------------
    // Apply filter to all motions
    //-------------------------------------
    throttle    = throttleFilter.filter(throttle);
    yaw         = yawFilter.filter(yaw);
    pitch       = pitchFilter.filter(pitch);
    roll        = rollFilter.filter(roll);

    //-------------------------------------
    // Finally, apply the motions
    //-------------------------------------
    navigate(throttle, yaw, pitch, roll);

    //-------------------------------------
    // we have used up the mouse input, if there was any.
    //-------------------------------------
    mouse_dx = 0.0f;
    mouse_dy = 0.0f;
    new_mouse_wheel_throttle = 0.0f;


    if (bSimulationPause) {
        // When simulation is paused, allow for manual time forward and reverse using key/buttons.
        // Filter the input for smooth start/stop.

        step_multiplier_input = 0.0f;

        if (bMinus) {
            step_multiplier_input = -3 * _stepMultiplier / FIR_WIDTH;
            if (bShiftModifier)
                step_multiplier_input *= 5;
        }
        else {
            if (bEquals) {
                step_multiplier_input = 3 * _stepMultiplier / FIR_WIDTH;
                if (bShiftModifier)
                    step_multiplier_input *= 5;
            }
        }


        if (bCtrlModifier)
            step_multiplier_input /= 5;

        _filteredStepMultiplier = stepMultiplierFilterWhenPaused.filter(step_multiplier_input);
        stepMultiplierFilter.filter(0.0f);                  // feed zeros to filter that isn't being used.

    } else {
        // simulation is not paused.

        stepMultiplierFilterWhenPaused.filter(0.0f);        // feed zeros to filter that isn't being used.

        if (bEquals) {
            step_multiplier_input = _stepMultiplier / 10.0f;
            if (bShiftModifier)
                step_multiplier_input *= 5;
            if (bCtrlModifier)
                step_multiplier_input /= 5;
        }
        else {
            if (bMinus) {
                step_multiplier_input = -_stepMultiplier / 10.0f;
                if (bShiftModifier)
                    step_multiplier_input *= 5;
                if (bCtrlModifier)
                    step_multiplier_input /= 5;
            }
            else {
                step_multiplier_input = _stepMultiplier / 30.0;
                // Don't use Shift modifier here or else using sideways shift movement using mouse would be impossible.
                if (bCtrlModifier)
                    step_multiplier_input /= 5;
            }
        }


        _filteredStepMultiplier = stepMultiplierFilter.filter(step_multiplier_input);
    }


    advanceScene(_filteredStepMultiplier * _stepMultiplierFrameRateAdjustment);

    //-------------------------------------
    // Manual movement of earth and moon in their orbit.
    // Intended to be used when simulation is paused or the earth/moon revolution is paused.
    float inc = 0.003f;
    if (bShiftModifier)
        inc *= 10;

    if (bAdvanceEarthInOrbit)
        earth._orbitalAngle += inc;
    if (bRetardEarthInOrbit)
        earth._orbitalAngle -= inc;
    if (bAdvanceMoonInOrbit)
        moon._orbitalAngle += inc;
    if (bRetardMoonInOrbit)
        moon._orbitalAngle -= inc;
    //-------------------------------------


    // always calculate sphere positions, even when simulation paused.
    // This is because ImGui may change variables of scene objects, and it must be
    // reflected by drawing updated objects.
    for (int i = 0; allSpheres[i] != NULL; i++)
    {
        allSpheres[i]->calculateCenterPosition();
    }

    if (bEarthSurfaceLockMode)
    {
        glm::mat4 emm = earth.getTransform();
        glm::vec3 xCenter = earth.getModelTransformedCenter();

        glm::vec4 S = glm::vec4(
            1.01f * earth._radius * sinf((float)space.rad(surfaceLockTheta)),
            0.0f,
            1.01f * earth._radius * cosf((float)space.rad(surfaceLockTheta)),
            1.0f);

        glm::vec3 xS = emm * S;
        glm::vec3 xD = emm * glm::vec4(S.x, S.y + 100.0f, S.z, 1.0f);
        //VECTOR downwardDirVector = VECTOR(xS, xCenter);
        //    space.crossProduct(VECTOR(xS, xCenter), VECTOR(xS, xD));

        //PNT xD PNT(xS.x, xS.y + 100, xS.z);

        //PNT newS = PNT(earth.getCenter()).translated(-followDistance, followVector);
        space.setFrame(
            AT_POINT,
            xS,
            VECTOR(xS, xD),
            PNT(xCenter));

    }
    else if (lockTarget != nullptr)
        LookAtTarget();

}

void Universe::clearAllFirFilters()
{
    throttleFilter.clear();
    yawFilter.clear();
    pitchFilter.clear();
    rollFilter.clear();
    stepMultiplierFilterWhenPaused.clear();
    stepMultiplierFilter.clear();
}


/*
  Immediately apply the given throttle, yaw, pitch & roll values to the Space frame.
  This changes view only, not anything to do with model.
 
 */
void Universe::navigate(float __throttle, float __yaw, float __pitch, float __roll)
{
    if (lockTarget == nullptr)
    {
        // Finally, Apply the motion value.
        // TODO - don't allow navigating into in a object.
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
        // TODO - don't allow navigating into in a object.
        if ((lockMode == TargetLockMode_OrientedViewTarget) || (lockMode == TargetLockMode_FollowTarget)) {
            if (__throttle != 0.0f) {
                followDistance -= __throttle;
            }
        }
        else
        {
            if (__throttle != 0.0f)
                space.moveFrame(Movement_Forward, __throttle);
        }

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

            space.rotateFrame(lockTarget->getCenter(), horizontalAngle, verticalAngle);

            //space.moveFrame(Movement_RightAlongSD, -__roll);

            if (lockMode == TargetLockMode_FollowTarget) {
                calculateFollowTargetLockVariables();
            }
            else if (lockMode == TargetLockMode_OrientedViewTarget) {
                // Observer may have moved due to navigation inputs, thereby chainging the orientation.
                // Re-calculate angles to be used to set observer's position in the next iteration.
                calculateOrientedViewLockVariables();
            }
        }
    }
}


int Universe::runMainLoop()
{
	ImGuiIO& io = ImGui::GetIO();
    SDL_Event event;

    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            // Always send mouse & keyboard events to ImGui
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
                    glViewport(0, 0, curWidth, curHeight);      // change viewport dimensions when window is resized
                    //glViewport(200, 200, 800, 600);
                }
                break;
            }

            //----------------------------------------------------
            // Pass mouse events to leela application only if ImGui isn't using them.
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
                        new_mouse_wheel_throttle = event.wheel.y * 100;
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
    }

	return 0;

}

int Universe::run()
{
    setvbuf(stdout, 0, _IONBF, 0);
    const char* glsl_version = "#version 330";

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("Leela", 500, 500, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    //window = SDL_CreateWindow("Leela", 300, 300, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    spdlog::info("Created SDL GL window");
    context = SDL_GL_CreateContext(window);


    glewInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

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


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    createFontCharacterTexture();

    SDL_GetWindowSize(window, &curWidth, &curHeight);
    spdlog::info("width = {}", curWidth);
    spdlog::info("height = {}", curHeight);
    
    spdlog::info("initializing scene objects... ");


    //---------------------------------------------------
    // Print OpenGL and Shading Language versions
    //---------------------------------------------------
    const GLubyte* glVersion;
    GLint glMajor, glMinor;
    
    glVersion = glGetString(GL_VERSION);
    glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
    glGetIntegerv(GL_MAJOR_VERSION, &glMinor);

    spdlog::info("OpenGL version string: {}", (char*)(glVersion));
    spdlog::info("OpenGL major = {}, minor = {}", glMajor, glMinor);
    spdlog::info("Shading language version string: {}", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    //---------------------------------------------------


	SDL_GL_SetSwapInterval(1);
	int retval = 0;
	try
	{
        compileShaders();
		initSceneObjectsAndComponents();
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

    cout << "Exiting leela with value " << retval;
    return retval;
}
