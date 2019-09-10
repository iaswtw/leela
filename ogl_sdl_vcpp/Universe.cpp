#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include "Universe.h"
#include "Utils.h"
#include <stdio.h>

Universe::Universe() :
    sunRenderer(sun),
    earthRenderer(earth),
    moonRenderer(moon)
{
}


Universe::~Universe()
{
}



/*************************************************************************************************


**************************************************************************************************/
void Universe::initSceneObjects()
{
    SetDefaultView();
    generateStars();


    axis.generateVertices(
        1600, 1600, 800,
        glm::vec3(0.2f, 0.2f, 0.5f),        // X axis color
        glm::vec3(0.2f, 0.5f, 0.2f),        // Y axis color
        glm::vec3(0.2f, 0.5f, 0.5f)         // Z axis color
    );
    // Sun
    //---------------------------------------
    //sun.setColor(0.7f, 0.7f, 0.1f);
    sun.setColor(1.0f, 1.0f, 0.6f);
    sun.setRotationParameters(140,          // radius
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
        0.003f,                             // rotation velocity
        glm::radians(270.0f),               // axis rotation angle
        glm::radians(23.5f)                 // axis tilt angle
    );
    earth.setOrbitalParameters(1400,        // radius of orbit
        glm::radians(0.0f),                 // initial orbital angle
        0.001f,                             // revolution velocity
        0.0f,                               // orbital rotation angle
        0                                   // orbital tilt
    );
    earth.setOrbitalPlaneColor(glm::vec3(0.55, 0.82, 1.0));

    // Moon
    //---------------------------------------
    moon.setColor(0.8f, 0.8f, 0.8f);
    moon.setRotationParameters(25,          // radius
        0,                                  // initial rotation angle
        0.005f,                              // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(10.0f)                 // axis tilt angle
    );
    moon.setOrbitalParameters(260,          // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                             // revolution velocity
        0,                                  // orbital rotation angle
        glm::radians(30.0)                  // orbital tilt
    );
    moon.setOrbitalPlaneColor(glm::vec3(0.3, 0.5, 0.3));

    moon.setParentSphere(&earth);
    earth.setParentSphere(&sun);
}


/*!
****************************************************************************



****************************************************************************/
void Universe::generateStars()
{
    /*  NOTE:
     *  max_z is the thickness of galaxy at a particular radius
     *  it is dynamically calculated for every randomly generated 'r'
     */


    int i;
    double max_dist;
    double max_radius;
    double max_theta;
    double max_z;
    double r, theta, phi;
    double w;
    unsigned char color;  // one component of color

    // generate cubically distributed stars
    i = 0;
    max_dist = 500000.0;
    while (1) {
        if (i == MAXSTARS)
            break;

        star[i].x = max_dist * double(rand()) / RAND_MAX - max_dist / 2;
        star[i].y = max_dist * double(rand()) / RAND_MAX - max_dist / 2;
        star[i].z = max_dist * double(rand()) / RAND_MAX - max_dist / 2;
        color = static_cast<unsigned char>(55) + static_cast<unsigned char>((double(rand()) / RAND_MAX) * 200);
        //star[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
        star[i].set_color(color, color, color);

        if (fabs(star[i].x) > 10000 ||
            fabs(star[i].y) > 10000 ||
            fabs(star[i].z) > 10000)
        {
            i++;
            starVertices.push_back(star[i].x);  starVertices.push_back(star[i].y);  starVertices.push_back(star[i].z);   starVertices.push_back(0.8);  starVertices.push_back(0.8);  starVertices.push_back(0.8);  starVertices.push_back(1.0);
        }
    }

    // generate stars for the galaxy
    max_radius = 1000000.0;
    max_theta = 2 * M_PI;
    i = 0;
    while (1) {
        if (i == .7 * MAXGALAXYSTARS)
            break;

        r = max_radius * (double(rand()) / RAND_MAX);
        theta = max_theta * (double(rand()) / RAND_MAX);
        w = r / max_radius;
        if (r < 40000)
            max_z = r;
        else
            max_z = 400000 * (1 / sqrt(1 + 20 * w*w));

        gstar[i].x = r * cos(theta);
        gstar[i].y = r * sin(theta);
        gstar[i].z = max_z * double(rand()) / RAND_MAX - max_z / 2;
        color = static_cast<unsigned char>(55) + static_cast<unsigned char>((double(rand()) / RAND_MAX) * 200);
        //gstar[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
        gstar[i].set_color(color, color, color);



        if (fabs(gstar[i].x) > 1000 ||
            fabs(gstar[i].y) > 1000 ||
            fabs(gstar[i].z) > 1000)
            i++;
    }

    while (1) {
        if (i == MAXGALAXYSTARS)
            break;

        r = 200000 * (double(rand()) / RAND_MAX);
        theta = M_PI * (double(rand()) / RAND_MAX);
        phi = 2 * M_PI * (double(rand()) / RAND_MAX);
        gstar[i].x = r * sin(theta) * cos(phi);
        gstar[i].y = r * sin(theta) * sin(phi);
        gstar[i].z = r * cos(theta);
        color = static_cast<unsigned char>((double(rand()) / RAND_MAX) * 255);
        //gstar[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
        gstar[i].set_color(color, color, color);
        i++;

    }

    // shift the Galaxy stars along x axis
    for (i = 0; i < MAXGALAXYSTARS; i++)
        gstar[i].x += max_radius * .66;


    // rotate all stars so that the the plane of the solar system
    // is perpendicular to the plane of the galaxy
    for (i = 0; i < MAXGALAXYSTARS; i++)
        gstar[i] = space.rotate(PNT(0, 0, 0), PNT(100, 0, 0), gstar[i], 80.0);


    gstarVertices.push_back(gstar[i].x);  gstarVertices.push_back(gstar[i].y);  gstarVertices.push_back(gstar[i].z);   gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(1.0);

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
void Universe::printShaderCompileStatus(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    printf("Shader compile status = %d\n", status);
    if (status == GL_TRUE)
    {
        printf("Compile successful.\n\n");
    }
    else
    {
        printf("!!! === Compile failed === !!!\n");

        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        printf("Log:\n%s\n", buffer);

        SDL_Quit();
        exit(1);
    }
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::readAndCompileShader(std::string filePath, GLuint &shaderId)
{
    std::string fileContents;
    std::string line;

    std::ifstream shaderFile(filePath);
    if (shaderFile.fail())
    {
        perror(filePath.c_str());
        printf("Failed to open %s\n", filePath.c_str());
        SDL_Quit();
        exit(1);
    }
    while (std::getline(shaderFile, line))
    {
        fileContents += line + "\n";
    }

    const char *contentsPtr = fileContents.c_str();
    glShaderSource(shaderId, 1, &contentsPtr, nullptr);
    glCompileShader(shaderId);

    // Check compile status
    printShaderCompileStatus(shaderId);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::compileShaders()
{
    //-------------------------------------------------------------
    // Vertex shader
    //-------------------------------------------------------------
    oglHandles.vertexShader = glCreateShader(GL_VERTEX_SHADER);
    printf("Loading and Compiling Vertex shader.\n");
    readAndCompileShader("shaders/shader.vert", oglHandles.vertexShader);

    //-------------------------------------------------------------
    // Fragment shader
    //-------------------------------------------------------------
    oglHandles.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    printf("Loading and Compiling Fragment shader.\n");
    readAndCompileShader("shaders/shader.frag", oglHandles.fragmentShader);

    //-------------------------------------------------------------
    // Shader program
    oglHandles.shaderProgram = glCreateProgram();
    glAttachShader(oglHandles.shaderProgram, oglHandles.vertexShader);
    glAttachShader(oglHandles.shaderProgram, oglHandles.fragmentShader);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::linkShaders()
{
    glLinkProgram(oglHandles.shaderProgram);

    // todo perform check to see if linking was successful
    GLint isLinked = 0;
    glGetProgramiv(oglHandles.shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(oglHandles.shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(oglHandles.shaderProgram, maxLength, &maxLength, &infoLog[0]);

        printf("Linker error log:\n");
        printf("%s\n", infoLog.data());

        // We don't need the program anymore.
        glDeleteProgram(oglHandles.shaderProgram);
    }
    else
    {
        printf("\nShader program link successful\n");
        glDetachShader(oglHandles.shaderProgram, oglHandles.vertexShader);
        glDetachShader(oglHandles.shaderProgram, oglHandles.fragmentShader);
    }

    // Don't leak shaders either.
    glDeleteShader(oglHandles.vertexShader);
    glDeleteShader(oglHandles.fragmentShader);

}


/*************************************************************************************************


**************************************************************************************************/
void Universe::useShaderProgram()
{
    glUseProgram(oglHandles.shaderProgram);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::unuseShaderProgram()
{
    glUseProgram(0);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::initializeGL()
{
    printf("Inside initializeGL\n");

    compileShaders();

    glBindAttribLocation(oglHandles.shaderProgram, 0, "position");
    glBindAttribLocation(oglHandles.shaderProgram, 1, "in_color");

    linkShaders();


    //---------------------------------------------------------------------------------------------------
    oglHandles.uniModel = getUniformLocation(oglHandles.shaderProgram, "model");
    oglHandles.uniView = getUniformLocation(oglHandles.shaderProgram, "view");
    oglHandles.uniProj = getUniformLocation(oglHandles.shaderProgram, "proj");

    oglHandles.uniMyCenterTransformed  = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.centerTransformed");
    oglHandles.uniMyRadius             = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.radius");
    oglHandles.uniMyIsValud            = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.isValid");
    oglHandles.uniMyIsLightSource      = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.isLightSource");

    oglHandles.uniSunCenterTransformed = getUniformLocation(oglHandles.shaderProgram, "sunCenterTransformed");
    oglHandles.uniSunRadius            = getUniformLocation(oglHandles.shaderProgram, "sunRadius");

    oglHandles.uniOtherSphereCenterTransformed = getUniformLocation(oglHandles.shaderProgram, "otherSphereCenterTransformed");
    oglHandles.uniOtherSphereRadius            = getUniformLocation(oglHandles.shaderProgram, "otherSphereRadius");


    oglHandles.posAttrib = getAttribLocation(oglHandles.shaderProgram, "position");
    oglHandles.colAttrib = getAttribLocation(oglHandles.shaderProgram, "in_color");



    //---------------------------------------------------------------------------------------------------
    // Axis
    glGenVertexArrays(1, &oglHandles.axisVao);
    glBindVertexArray(oglHandles.axisVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * axis.getVertices().size(),
        axis.getVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Cube stars
    glGenVertexArrays(1, &oglHandles.starsVao);
    glBindVertexArray(oglHandles.starsVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * starVertices.size(),
        starVertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------
    // galaxy stars
    glGenVertexArrays(1, &oglHandles.gstarsVao);
    glBindVertexArray(oglHandles.gstarsVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * gstarVertices.size(),
        gstarVertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------



    sunRenderer.constructVerticesAndSendToGpu(oglHandles);
    earthRenderer.constructVerticesAndSendToGpu(oglHandles);
    moonRenderer.constructVerticesAndSendToGpu(oglHandles);

    earthRenderer.bShowLatitudesAndLongitudes = true;
    sunRenderer.setAsLightSource();

    glBindVertexArray(0);       // Disable VBO


    // Was using the below commented before switching to Space class of the old universe program
    //viewMatrix = glm::lookAt(
    //    glm::vec3(1600.0f, 1600.0f, 1600.0f),
    //    glm::vec3(0.0f, 0.0f, 0.0f),
    //    glm::vec3(0.0f, 0.0f, 1.0f));



    // The following can be used to look at earth at roughly 270 degree position for the purpose of viewing hybrid eclipse.
    //viewMatrix = glm::lookAt(
    //    //            glm::vec3(1600.0f, 1600.0f, 1600.0f),
    //    glm::vec3(-200.0f, -700.0f, 200.0f),
    //    //glm::vec3(0.0f, 0.0f, 0.0f),
    //    glm::vec3(-650.0f, -900.0f, -100.0f),
    //    glm::vec3(0.0f, 0.0f, 1.0f));


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
    case SDLK_c:
        NavigationLockOntoSun(UCmdParam_Toggle);
        break;
    case SDLK_d:
        SetDefaultView();
        break;
    case SDLK_f:
        FastForward(UCmdParam_Start);
        break;
    case SDLK_g:
        Earth_OrbitalPlane(UCmdParam_Toggle);
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
        Earth_SetOrbitalPositionAngle(M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_2:
        Earth_SetOrbitalPositionAngle(M_PI);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_3:
        Earth_SetOrbitalPositionAngle(3 * M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_4:
        Earth_SetOrbitalPositionAngle(0);
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


    // Arrow keys
    case SDLK_UP:
        _stepMultiplier *= 1.6666;
        break;
    case SDLK_DOWN:
        _stepMultiplier *= 0.6;
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
            roll = nominalRoll;
        break;
    case SDLK_INSERT:
        if (!bAltModifier)
            pitch = -nominalPitch; 
        else
            roll = -nominalRoll;
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
    case SDLK_ESCAPE:
        bMouseGrabbed = false;
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
        break;
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

void Universe::onMouseMotion(SDL_Event* event)
{
    float dx = event->motion.xrel;
    float dy = event->motion.yrel;

    if (bCtrlModifier)
    {
        dx /= 15.0f;
        dy /= 15.0f;
    }

    //printf("dx = %d, dy = %d\n", dx, dy);

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


    while (1)
    {
        if (bLeftMouseButtonDown) {
            space.moveFrame(Movement_Forward, -dy * 5);


            if (bSidewaysMotionMode == 1) {
                space.moveFrame(Movement_RotateRight, 90);
                space.moveFrame(Movement_Forward, dx);
                space.moveFrame(Movement_RotateLeft, 90);
            }
            else {
                space.moveFrame(Movement_RotateRight, dx / 10.0);
            }
            break;
        }

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
        space.setFrame(AT_POINT,
            space.S,
            VECTOR(space.S, earth.getCenter()),
            PNT(space.S.x, space.S.y, space.S.z - 100));
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

        space.setFrame(AT_POINT,
            space.S,
            VECTOR(space.S, sun.getCenter()),
            PNT(space.S.x, space.S.y, space.S.z - 100));
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
        _stepMultiplier = 0.005;
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
    F_REFERENCE_VECTOR_ALONG_Z = 0;
    bLockOntoEarth = false;

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
void Universe::Earth_SetOrbitalPositionAngle(double fAngle)
{
    earth.setOrbitalAngle(fAngle);

    /* Even though the earth's coordinates would be calculated from its
       orbit angle (tho) in the advance() function, it won't happen if
       the simulation is paused.  Hence we need to do it here. */
    //earth.C.x = earth.R * cos(earth.tho);
    //earth.C.y = earth.R * sin(earth.tho);
    //earth.C.z = 0;

    earth.calculateCenterPosition();
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
void Universe::Moon_SetOrbitalPositionAngle(double fAngle)
{
    moon.setOrbitalAngle(fAngle);
    moon.calculateCenterPosition();
}


/*************************************************************************************************


**************************************************************************************************/
void Universe::render()
{
    //glClearDepth(1.0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    useShaderProgram();

    glUniform3fv(
        oglHandles.uniSunCenterTransformed,
        1,
        glm::value_ptr(sun.getModelTransformedCenter())
    );

    glUniform1f(oglHandles.uniSunRadius, sun.getRadius());


    //=====================================================================================
    // View and projection remain same for the entire scene
    //=====================================================================================
    // View transformation
    //----------------------------------------------
    glUniformMatrix4fv(oglHandles.uniView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

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

    glUniformMatrix4fv(oglHandles.uniProj, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    //=====================================================================================

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //----------------------------------------------
    // Axis model transformation
    //----------------------------------------------
    glUniformMatrix4fv(
        oglHandles.uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(glm::mat4(1.0))
    );

    glUniform1i(oglHandles.uniMyIsValud, false);
    // ideally, after setting IsValid to false, no need to set the other variables to draw the axis.
    glUniform1i(oglHandles.uniMyIsLightSource, 0);
    glUniform3f(oglHandles.uniMyCenterTransformed, 0.0f, 0.0f, 0.0f);

    glBindVertexArray(oglHandles.axisVao);

    // Draw vertices
    glDrawArrays(GL_LINES, 0, axis.getVertices().size() / 7);


    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //----------------------------------------------
    // Cubic stars model transformation
    //----------------------------------------------
    glUniformMatrix4fv(
        oglHandles.uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(glm::mat4(1.0))
    );

    glUniform1i(oglHandles.uniMyIsValud, false);
    glBindVertexArray(oglHandles.starsVao);

    //printf("num points in startVertices = %d\n", starVertices.size());
    // Draw vertices
    glDrawArrays(GL_POINTS, 0, starVertices.size() / 7);

    ////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ////----------------------------------------------
    //// Galaxy stars model transformation
    ////----------------------------------------------
    //glUniformMatrix4fv(
    //    oglHandles.uniModel,
    //    1,
    //    GL_FALSE,
    //    glm::value_ptr(glm::mat4(1.0))
    //);

    //glUniform1i(oglHandles.uniMyIsValud, false);
    //glUniform3fv(
    //    oglHandles.uniOtherSphereCenterTransformed,
    //    1,
    //    glm::value_ptr(moon.getModelTransformedCenter())
    //);

    //glBindVertexArray(oglHandles.gstarsVao);

    //// Draw vertices
    //glDrawArrays(GL_POINT, 0, gstarVertices.size() / 7);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    earthRenderer.render(oglHandles, &moon);
    moonRenderer.render(oglHandles, &earth);
    sunRenderer.render(oglHandles, nullptr);


    glBindVertexArray(0);

    unuseShaderProgram();

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
    if (__throttle != 0.0f)
        space.moveFrame(Movement_Forward, __throttle);
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


}

/*************************************************************************************************


**************************************************************************************************/
int Universe::run()
{
    setvbuf(stdout, 0, _IONBF, 0);

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    printf("Created SDL GL window\n");
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    glewInit();

    SDL_GetWindowSize(window, &curWidth, &curHeight);
    printf("width = %d\n", curWidth);
    printf("height = %d\n", curHeight);


    printf("initializing scene objects... ");
    initSceneObjects();
    initializeGL();
    printf("done\n");


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);


    SDL_GetMouseState(&previousX, &previousY);

    SDL_Event event;
    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:           bQuit = true;                  break;
            case SDL_KEYDOWN:
                onKeyDown(&event);
                break;
            case SDL_KEYUP:
                onKeyUp(&event);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (!bMouseGrabbed) {
                    bMouseGrabbed = true;
                    SDL_ShowCursor(SDL_DISABLE);
                    //SDL_WarpMouseInWindow(window, curWidth / 2, curHeight / 2);
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:   bLeftMouseButtonDown = true;    break;
                case SDL_BUTTON_RIGHT:  bRightMouseButtonDown = true;   break;
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
                {
                    onMouseMotion(&event);
                }
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    curWidth = event.window.data1;
                    curHeight = event.window.data2;

                    printf("width = %d\n", curWidth);
                    printf("height = %d\n", curHeight);
                    glViewport(0, 0, curWidth, curHeight);
                }
                break;
            }

            if (bQuit)
                break;

        } // while SDL event poll


        if (bQuit)
            break;


        processFlags();

        render();

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }

    SDL_Quit();
    return 0;
}
