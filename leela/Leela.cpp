#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include "Leela.h"
#include "Utils.h"
#include <stdio.h>
#include <string>
#include "Elements.h"
#include "ViewportBorderRenderer.h"

#include <spdlog/spdlog.h>

#define _USE_MATH_DEFINES
#include <math.h>


// Following function was copied from imgui_demo.cpp
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
void Leela::HelpMarker(const char* desc)
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


Leela::Leela() :

    starsRenderer(stars),

    throttleFilter                  (FIR_WIDTH, fir_coeff),
    yawFilter                       (FIR_WIDTH, fir_coeff),
    pitchFilter                     (FIR_WIDTH, fir_coeff),
    rollFilter                      (FIR_WIDTH, fir_coeff),
    stepMultiplierFilterWhenPaused  (FIR_WIDTH, fir_coeff),
    stepMultiplierFilter            (FIR_WIDTH, fir_coeff)
{
}


Leela::~Leela()
{
}

void Leela::compileShaders()
{
    struct ShaderProgramInfo
    {
        GlslProgramType type;
        std::string vertexShaderFilename;
        std::string fragmentShaderFilename;
    };

    ShaderProgramInfo shaderProgInfo[] = {
        // program type enum                vertex shader filename           fragment shader filename
        { GlslProgramType::Planet,          "planet.vert.glsl",               "planet.frag.glsl"                  },
        { GlslProgramType::Sun,             "sun.vert.glsl",                  "sun.frag.glsl"                     },
        { GlslProgramType::Star,            "star.vert.glsl",                 "star.frag.glsl"                    },
        { GlslProgramType::Simple,          "simple.vert.glsl",               "simple.frag.glsl"                  },
        { GlslProgramType::SimpleOrtho,     "simple_ortho.vert.glsl",         "simple_ortho.frag.glsl"            },
        { GlslProgramType::Font,            "font.vert.glsl",                 "font.frag.glsl"                    },
        { GlslProgramType::BookmarkSphere,  "bookmark.vert.glsl",             "bookmark.frag.glsl"                }
    };
    
    spdlog::info("Compiling all GLSL programs");

    for (auto si : shaderProgInfo)
    {
        auto prog = new GlslProgram(si.type);
        prog->compileShadersFromFile(si.vertexShaderFilename.c_str(), si.fragmentShaderFilename.c_str());
        prog->link();

        shaderPrograms.push_back(prog);
    }

}

/*************************************************************************************************
 Initialize various parameters of planets, stars, etc.  Parameters include radius of objects,
 time periods of rotation and revolution, colors, etc.
**************************************************************************************************/
void Leela::initSceneObjectsAndComponents()
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


    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

    // For each planet entry in the table, create a Spherical body object and configure it.
    //      - Create an appropriate renderer object for it.
    //      - do special handling for some spheres such as earth, moon, and sun.
    PlanetInfo* _planetInfo = nullptr;
    int _numPlanetInfo = 0;

    if (USE_TO_SCALE) {
        _planetInfo = &planetInfoToScale[0];
        _numPlanetInfo = numPlanetInfoToScale;
    }
    else {
        _planetInfo = &planetInfo[0];
        _numPlanetInfo = numPlanetInfo;
    }


    //for (PlanetInfo pi : planetInfo)
    for (int i = 0; i < _numPlanetInfo; i++)
    {
        PlanetInfo &pi = _planetInfo[i];

        SphericalBody * sb = new SphericalBody(pi.name);
        sb->setRotationParameters(pi.radius,
                                  glm::radians(pi.rotationAngle),
                                  pi.rotationAngularVelocity,
                                  glm::radians(pi.axisTiltOrientationAngle),
                                  glm::radians(pi.axisTiltAngle)
        );
        sb->setOrbitalParameters(pi.orbitalRadius,
                                 glm::radians(pi.orbitalAngle),
                                 pi.orbitalAngularVelocity,
                                 glm::radians(pi.nodalPrecessionInitialAngle),
                                 glm::radians(pi.orbitalPlaneTiltAngle)
        );
        sb->setColor(pi.color);

        //------------------------------
        // Create and configure renderer
        //------------------------------

        Renderer* renderer = nullptr;
        PlanetRenderer* planetRenderer = nullptr;
        SphericalBodyRenderer* sphericalBodyRenderer = nullptr;

        if (pi.name == "Sun") {
            sunRenderer = new SunRenderer(pi.textureFilename);
            renderer = sphericalBodyRenderer = sunRenderer;             // initialize superclass handles for later use in this function
        }
        else {
            planetRenderer = new PlanetRenderer(pi.textureFilename);
            renderer = sphericalBodyRenderer = planetRenderer;          // initialize superclass handles for later use in this function
        }
        
        sphericalBodyRenderer->setPolygonCountLevel(pi.polygonCountLevel);

        //----------------------------------------------------
        if (pi.name == "Sun") {
            sun = sb;
            sunRenderer->setAsLightSource();
        }
        else if (pi.name == "Mercury") {
            mercury = sb;
        }
        else if (pi.name == "Earth") {
            earth = sb;
            earthRenderer = planetRenderer;
            sphericalBodyRenderer->bShowOrbit = true;

            // TODO: We would rather create the lat-lon renderer here.  But if done so, its rotation angle isn't correct.
            //       It somehow seems to be dependent on some calculated variable in the sphere which gets updated
            //       after initializing the main renderer several lines below.
            // 
            earthLatLonRenderer = new LatLonRenderer();
            earth->addComponent(earthLatLonRenderer);
            earthLatLonRenderer->init();
        }
        else if (pi.name == "Moon") {
            moon = sb;
            moonRenderer = planetRenderer;
            sphericalBodyRenderer->bShowOrbit = true;
        }
        else if (pi.name == "Mars") {
            mars = sb;
            marsRenderer = planetRenderer;
        }
        else if (pi.name == "Jupiter") {
            jupiter = sb;
        }
        else if (pi.name == "Uranus") {
            uranus = sb;
        }
        else {
        }

        //----------------------------------------------------
        // this assums sun was created before other bodies in the system
        if (pi.name != "Sun")
            sb->setSunSphere(sun);

        sb->addComponent(renderer);         // Spherical body now owns the renderer.
        renderer->init();                   // construct spheres and send data to GPU


        // TODO: See todo above commented code that creates lat-lon renderer.
        //if (pi.name == "Earth") {
        //    earthLatLonRenderer = new LatLonRenderer();
        //    earth->addComponent(earthLatLonRenderer);
        //    earthLatLonRenderer->init();
        //}

        //----------------------------------------------------
        
        // find parent scene object and the newly created scene object as its child.
        SceneObject* parentObj = &scene;
        if (pi.parentName != "") {
            parentObj = SceneObject::getSceneObjectByName(&scene, pi.parentName);
        }
        // todo: assert on parentObj
        parentObj->addSceneObject(sb);

    }

    //----------------------------------------------------------------------
    
    // Set "related" sphere.  This can only be done after creating all spheres to ensure "related" sphere
    // has been created. (Releated sphere can be a child sphere; and the child won't exist if parent were
    // to find its "other" sphere in the above loop.)
    //
    for (int i = 0; i < _numPlanetInfo; i++)
    {
        PlanetInfo &pi = _planetInfo[i];
        SphericalBody* sb = dynamic_cast<SphericalBody*>(
                                SceneObject::getSceneObjectByName(&scene, pi.name)
                            );
        if (sb)
        {
            // set related object.  Only 1 related object is supported at the moment.
            for (std::string relatedObjName : pi.relatedObjectNames)
            {
                SphericalBody* relatedSphere =  dynamic_cast<SphericalBody*>(
                                                    SceneObject::getSceneObjectByName(&scene, relatedObjName)
                                                );
                
                sb->setRelatedSphere(relatedSphere);

                if (relatedSphere)
                    printf("Setting %s as other sphere for %s", relatedSphere->_name.c_str(), sb->_name.c_str());
                //spdlog::info("Setting {} as other sphere for {}", fmt::ptr(relatedSphere->_name.c_str()), fmt::ptr(sb->_name.c_str()));

                break;      // shader supports only 1 related scene object
            }
        }
        else {
            // todo print warning that expected scene object wasn't found.
        }
    }

    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

    SetApplicationStartView();

    coordinateAxisRenderer.init();

    constructFontInfrastructureAndSendToGpu();

    //-----------------------------------------------

    monthLabelsRenderer = new MonthLabelsRenderer();
    earth->addComponent(monthLabelsRenderer);
    monthLabelsRenderer->init();

    //---------------------------------------------------------------------------------------------------
    // Bookmarks for various position on earth

    typedef struct {
        std::string label;
        float lat;
        float lon;
    } BookmarkInfo;


    BookmarkInfo bminfo[] = {
        {"Austin", 30.2f, -97.7},
        {"Chicago", 41.8f, -87.6f},
        {"Cheyenne", 41.1f, -104.8f},
        {"Fairbanks", 64.8f, -147.7f},
        {"Nuuk", 64.17f, -51.73f}
    };

    for (auto _bminfo : bminfo)
    {
        auto bm = new Bookmark();
        bm->init();
        bm->set(_bminfo.label, _bminfo.lat, _bminfo.lon);
        earth->addSceneObject(bm);               // bm is owned by earth scene object after this call.

        auto bmr = new BookmarkRenderer();
        bm->addComponent(bmr);
        bmr->init();
    }


    //---------------------------------------------------------------------------------------------------
    
    scene.addComponent(&starsRenderer);
    starsRenderer.init();

    //------------------------------------------------------------------------
    // Viewport-related
    
    minimapViewport                 = new ViewportSceneObject(ViewportType::Minimap);
    alternateObserverViewport       = new ViewportSceneObject(ViewportType::AlternateObserver);

    scene.addSceneObject(minimapViewport);
    scene.addSceneObject(alternateObserverViewport);

    minimapViewport->addComponent(new ViewportBorderRenderer());
    alternateObserverViewport->addComponent(new ViewportBorderRenderer());

    minimapViewport->setDimensions(10, 50, 400, 300);
    minimapViewport->bEnabled = true;

    glBindVertexArray(0);       // Disable VBO

    //-------------------------------------------------------------------------
    // Finally, print the constructed scene
    //-------------------------------------------------------------------------
    SceneObject::printTree(&scene);
}


void Leela::printGlError()
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

void Leela::_advanceSceneObject(SceneObject* sceneObject, float stepMultiplier)
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

void Leela::advanceScene(float stepMultiplier)
{
    _advanceSceneObject(&scene, stepMultiplier);
}


void Leela::ChangeSidewaysMotionMode()
{
    bSidewaysMotionMode = !bSidewaysMotionMode;
}

void Leela::SetDefaultView()
{
    space.pushFrame();

    space.initFrame();
    space.rotateFrame(PNT(0, 0, 0), 0, -15.0);
    //space.rotateFrame(PNT(0, 0, 0), 0, 0);

    space.moveFrame(Movement_Backward, 7000.0f);

    ResetFollowTargetAndMode();

}

void Leela::SetApplicationStartView()
{
    space.pushFrame();
    space.initFrame();

    Earth_SetOrbitalPositionAngle(3 * M_PI / 8);

    PNT newS = PNT(-266.775859, 2177.232389, 266.224952);
    space.setFrame(AT_POINT,
        newS,
        VECTOR(newS, earth->getCenter()),
        PNT(newS.x, newS.y, newS.z - 100));

    // Adjust navigation view locks on earth and sun
    SetLockTargetAndMode(earth, TargetLockMode_ViewTarget);
    earth->bRevolutionMotion = true;
}


void Leela::SetLockTargetAndMode(SphericalBody* target, TargetLockMode mode)
{
    lockTarget = target;

    // follow target has to be set before setting mode.
    SetLockMode(mode);
}

void Leela::SetLockMode(TargetLockMode mode)
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

void Leela::calculateCommonTargetLockVariables()
{
    PNT T = lockTarget->getCenter();
    PNT S = space.S;
    followDistance = float(S.distanceTo(T));

    spdlog::info("followDistance = {}", followDistance);

}

void Leela::calculateFollowTargetLockVariables()
{
    PNT T = lockTarget->getCenter();
    PNT S = space.S;
    followVector = VECTOR(S, T);
}

//
// Calculate `orientedTargetLock_alpha` and `orientedTargetLock_beta` members.
//
void Leela::calculateOrientedViewLockVariables()
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

void Leela::cycleLockMode()
{
    if (lockMode == TargetLockMode_ViewTarget)
        SetLockMode(TargetLockMode_FollowTarget);
    else if (lockMode == TargetLockMode_FollowTarget)
        SetLockMode(TargetLockMode_OrientedViewTarget);
    else
        SetLockMode(TargetLockMode_ViewTarget);
}

void Leela::cycleLabelModes()
{
    if (!bShowMonthNames && !bShowLargeLabels) {
        bShowMonthNames = true;
    }
    else if (bShowMonthNames && !bShowLargeLabels) {
        bShowLargeLabels = true;
    }
    else {
        bShowMonthNames = false;
        bShowLargeLabels = false;
    }

}


void Leela::hideMonthLabels()
{
    bShowMonthNames = false;
}

void Leela::showLargeMonthLabels()
{
    bShowMonthNames = true;
    bShowLargeLabels = true;
}

void Leela::showNormalMonthLabels()
{
    bShowMonthNames = true;
    bShowLargeLabels = false;
    bMonthLabelsCloserToSphere = false;
    bShowLabelsOnTop = false;
}


void Leela::ResetFollowTargetAndMode()
{
    SetLockTargetAndMode(nullptr, TargetLockMode_ViewTarget);
}

void Leela::ToggleFollowTarget(SphericalBody* target, TargetLockMode mode)
{
    if (lockTarget == target)
        lockTarget = nullptr;
    else
        lockTarget = target;

    SetLockMode(mode);
}

void Leela::LookAtTarget()
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
void Leela::IncreaseSimulationSpeed()
{
    if (_stepMultiplier < 400.0f)
        _stepMultiplier *= 2.0f;
}

/*
 Halve the speed each time this is called.
 */
void Leela::DecreaseSimulationSpeed()
{
    if (_stepMultiplier > 0.0001666)
        _stepMultiplier *= 0.5f;
}

/*!
****************************************************************************

 Set simulation speed to one of the 5 hardcoded values.

****************************************************************************/
void Leela::SetSimulationSpeed(int nParam)
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
void Leela::SetTimeDirection(int nParam)
{
    eTimeDirection = (UTimeDirectionType)nParam;
}

// Turn On/Off Fast forward motion.
void Leela::FastForward(int nParam)
{
    ChangeBoolean(&bEquals, nParam);
}

// Turn On/Off fast Reverse motion.
void Leela::Rewind(int nParam)
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
void Leela::ChangeBoolean(bool *pBool, int nParam)
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

void Leela::SimulationPause(int nParam)
{
    ChangeBoolean(&bSimulationPause, nParam);
}

void Leela::Earth_RotationMotion(int nParam)
{
    ChangeBoolean(&earth->bRotationMotion, nParam);
}

void Leela::Earth_RevolutionMotion(int nParam)
{
    ChangeBoolean(&earth->bRevolutionMotion, nParam);
    //F_REFERENCE_VECTOR_ALONG_Z = 0;
    //bLockOntoEarth = false;
}


void Leela::Earth_PrecessionMotion(int nParam)
{
    if (nParam == UCmdParam_Reset)
    {
        earth->bPrecessionMotion = false;
        earth->_axisTiltOrientationAngle = glm::radians(0.0f);
    }
    else
    {
        ChangeBoolean(&earth->bPrecessionMotion, nParam);
    }
}

void Leela::Earth_OrbitalPlane(int nParam)
{
    ChangeBoolean(&earthRenderer->bShowOrbitalPlane, nParam);
}

void Leela::Earth_Orbit(int nParam)
{
    ChangeBoolean(&earthRenderer->bShowOrbit, nParam);
}

void Leela::Moon_OrbitalPlane(int nParam)
{
    ChangeBoolean(&moonRenderer->bShowOrbitalPlane, nParam);
}


void Leela::Moon_Orbit(int nParam)
{
    ChangeBoolean(&moonRenderer->bShowOrbit, nParam);
}

void Leela::Moon_ResetNodalPrecession()
{
    moon->_nodalPrecessionAngle = 0.0f;
}

bool Leela::IsAnyOrbitVisible()
{
    return _IsAnyOrbitVisible(&scene);
}

bool Leela::_IsAnyOrbitVisible(SceneObject * sceneObject)
{
    for (Renderer* r : sceneObject->_renderers)
    {
        SphericalBodyRenderer* sbr = dynamic_cast<SphericalBodyRenderer*>(r);
        if (sbr) {
            if (sbr->bShowOrbit)
                return true;        // if orbit is visible for this scene object, break the recursion.
                                    // No need to test any child scene objects.
        }
    }

    for (SceneObject* obj : sceneObject->_childSceneObjects)
    {
        if (_IsAnyOrbitVisible(obj))
            return true;
    }

    return false;
}

void Leela::DisableShowingAllOrbits()
{
    _DisableShowingOrbit(&scene);
}

void Leela::_DisableShowingOrbit(SceneObject* sceneObject)
{
    for (Renderer* r : sceneObject->_renderers)
    {
        SphericalBodyRenderer * sbr = dynamic_cast<SphericalBodyRenderer*>(r);
        if (sbr) {
            sbr->bShowOrbit = false;
        }
    }
    for (SceneObject* obj : sceneObject->_childSceneObjects)
    {
        _DisableShowingOrbit(obj);
    }
}

void Leela::ShowAllOrbits()
{
    vector<string> names;
    _ShowAllOrbits(&scene, names);
}

//
// If `names` is empty, all spherical bodies will have their orbits turned on.
// If `names` is not empty, only the spherical bodies whose names are in `names` vector will have orbits turned on.
//
void Leela::_ShowAllOrbits(SceneObject * sceneObject, vector<string>& names)
{
    for (Renderer* r : sceneObject->_renderers)
    {
        SphericalBodyRenderer* sbr = dynamic_cast<SphericalBodyRenderer*>(r);
        if (sbr) {
            bool doChange = true;           // assume empty `names`

            if (names.size()) {
                doChange = false;           // default to false now

                for (string name : names) {
                    if (name == sbr->_sphere->name()) {
                        doChange = true;
                        break;
                    }
                }
            }

            if (doChange)
                sbr->bShowOrbit = true;
        }
    }
    for (SceneObject* obj : sceneObject->_childSceneObjects)
    {
        _ShowAllOrbits(obj, names);
    }
}


void Leela::ShowEarthAndMoonOrbits()
{
    vector<string> names = {"Earth", "Moon"};
    _ShowAllOrbits(&scene, names);
}


void Leela::Moon_OrbitalPlaneRotation(int nParam)
{

    if (nParam == UCmdParam_Reset)
    {
        moon->bOrbitalPlaneRotation = false;
        moon->_nodalPrecessionAngle = glm::radians(0.0f);
    }
    else
    {
        ChangeBoolean(&moon->bOrbitalPlaneRotation, nParam);
    }
}

void Leela::Moon_RevolutionMotion(int nParam)
{
    ChangeBoolean(&moon->bRevolutionMotion, nParam);
}

void Leela::SetDotDensity(int nParam)
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


void Leela::toggleFullScreen()
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

void Leela::toggleControlPanelVisibilityWhenMouseGrabbed()
{
    bAlwaysShowControlPanel = !bAlwaysShowControlPanel;
}

void Leela::cleanupAndExitApplication()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void Leela::Moon_SetOrbitalPositionAngle(double fAngle)
{
    moon->setOrbitalAngle(float(fAngle));
}

void Leela::Earth_SetOrbitalPositionAngle(double fAngle)
{
    earth->setOrbitalAngle(float(fAngle));
}


void Leela::setWidgetControlMode()
{
    SDL_ShowCursor(SDL_ENABLE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void Leela::resetWidgetControlMode()
{
    //SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

//
// - Process flags set in response to key presses.
// - Process mouse movements.
//
void Leela::processFlags()
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
        if (bAltModifier) {
            if (USE_TO_SCALE)
                throttle *= 100;             // shift + alt = super fast zoom
            else
                throttle *= 100;             // shift + alt = super fast zoom
        }
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
        // process time speed-up and slow-down flags.

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

    // - advance is called even if scene is paused.
    //      - _filteredStepMultiplier will eventually become zero if simulation is paused.
    // - positions/orientations etc. are calculated as part of advancing (even if the increment is zero).
    //      - This is necessary to draw updated objects if their are manipulated through ImGui widgets.
    advanceScene(_filteredStepMultiplier * _stepMultiplierFrameRateAdjustment);

    //-------------------------------------
    // Manual movement of earth and moon in their orbit.
    // Intended to be used when simulation is paused or the earth/moon revolution is paused.
    float inc = 0.003f;
    if (bShiftModifier)
        inc *= 10;

    if (bAdvanceEarthInOrbit)
        earth->_orbitalAngle += inc;
    if (bRetardEarthInOrbit)
        earth->_orbitalAngle -= inc;
    if (bAdvanceMoonInOrbit)
        moon->_orbitalAngle += inc;
    if (bRetardMoonInOrbit)
        moon->_orbitalAngle -= inc;
    //-------------------------------------

    if (bEarthSurfaceLockMode)
    {
        glm::mat4 emm = earth->getTransform();
        glm::vec3 xCenter = earth->getModelTransformedCenter();

        // S has x and z components. y = 0. S is on 180 degree meridian.
        PNT S = PNT(
            1.01f * earth->_radius * sinf((float)space.rad(surfaceLockTheta)),
            0.0f,
            1.01f * earth->_radius * cosf((float)space.rad(surfaceLockTheta)));
        
        PNT D = S.translated(100.0, VECTOR(PNT(0,0,0), PNT(0.0, 100.0, 0.0)));

        glm::vec3 xS = emm * glm::vec4(S.toVec3(), 1.0);
        //glm::vec3 xD = emm * glm::vec4(S.x, S.y + 100.0f, S.z, 1.0f);
        glm::vec3 xD = emm * glm::vec4(space.D.toVec3(), 1.0);
        
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

void Leela::clearAllFirFilters()
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
void Leela::navigate(float __throttle, float __yaw, float __pitch, float __roll)
{
    bool bApplyThrottle = false;


    if (bEarthSurfaceLockMode)
    {
        if ((__yaw != 0.0f) || (__pitch != 0.0f))
        {
            space.moveFrame(Movement_RotateRight, __yaw);
            space.moveFrame(Movement_RotateUp, __pitch);
        }
    }
    else if (lockTarget == nullptr)
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
        // don't allow navigating into in a object using throttle.
        float distSC = glm::distance(lockTarget->getCenter(), space.S.toVec3());          // distance from camera to center of lock target

        if ((__throttle < 0) ||                                                         // moving away from the object.
            ((__throttle > 0) && (distSC > (1 + __throttle + lockTarget->_radius)))) {      // moving towards object, and we won't cross into the object if we did.
            bApplyThrottle = true;
        }

        if (bApplyThrottle) {

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
        }

        // TODO: prevent navigating into the object using using sideways mode or D-mode rotation
        
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


int Leela::runMainLoop()
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
                    //primaryViewport->setDimensions(0, 0, curWidth, curHeight);
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

        {
            GLint renderbufferID;
            GLint depthSize;
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &renderbufferID);

            glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &depthSize);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            std::cout << "Depth buffer size: " << depthSize << " bits" << std::endl;
        }


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    return 0;

}

int Leela::run()
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
    //std::string fullFontFilePath = FindFontFile("Arial.ttf");
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
