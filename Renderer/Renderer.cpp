// mantle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "Renderer.h"
#include "UCore.h"



/*************************************************************************************************


**************************************************************************************************/
GLint getUniformLocation(GLuint shaderProgram, const std::string& uniformName)
{
    GLint id = glGetUniformLocation(shaderProgram, uniformName.c_str());
    if (id < 0)
    {
        printf("Error getting location of uniform variable '%s'", uniformName.c_str());
        SDL_Quit();
        exit(1);
    }
    else
    {
        printf("%s = %d\n", uniformName.c_str(), id);
    }

    return id;
}

/*************************************************************************************************


**************************************************************************************************/
GLint getAttribLocation(GLuint shaderProgram, const std::string& attribName)
{
    GLint id = glGetAttribLocation(shaderProgram, attribName.c_str());
    if (id < 0)
    {
        printf("Error getting location of attrib variable '%s'", attribName.c_str());
        SDL_Quit();
        exit(1);
    }
    else
    {
        printf("%s = %d\n", attribName.c_str(), id);
    }

    return id;
}


Renderer::Renderer(Universe& core) :
    _core(core),
    sunRenderer(core.sun),
    earthRenderer(core.earth),
    moonRenderer(core.moon)
{
}

Renderer::~Renderer()
{

}


/*************************************************************************************************


**************************************************************************************************/
void Renderer::printGlError()
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
void Renderer::printShaderCompileStatus(GLuint shader)
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
void Renderer::readAndCompileShader(std::string filePath, GLuint &shaderId)
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
void Renderer::compileShaders()
{
    //-------------------------------------------------------------
    // Vertex shader
    //-------------------------------------------------------------
    oglHandles.vertexShader = glCreateShader(GL_VERTEX_SHADER);
    printf("Loading and Compiling Vertex shader.\n");
    readAndCompileShader("../ucore/shaders/shader.vert", oglHandles.vertexShader);

    //-------------------------------------------------------------
    // Fragment shader
    //-------------------------------------------------------------
    oglHandles.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    printf("Loading and Compiling Fragment shader.\n");
    readAndCompileShader("../ucore/shaders/shader.frag", oglHandles.fragmentShader);

    //-------------------------------------------------------------
    // Shader program
    oglHandles.shaderProgram = glCreateProgram();
    glAttachShader(oglHandles.shaderProgram, oglHandles.vertexShader);
    glAttachShader(oglHandles.shaderProgram, oglHandles.fragmentShader);
}

/*************************************************************************************************


**************************************************************************************************/
void Renderer::linkShaders()
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
void Renderer::useShaderProgram()
{
    glUseProgram(oglHandles.shaderProgram);
}

/*************************************************************************************************


**************************************************************************************************/
void Renderer::unuseShaderProgram()
{
    glUseProgram(0);
}

/*************************************************************************************************


**************************************************************************************************/
void Renderer::initializeGL()
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

    oglHandles.uniNightColorMultiplier = getUniformLocation(oglHandles.shaderProgram, "nightColorMultiplier");

    oglHandles.uniMyCenterTransformed = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.centerTransformed");
    oglHandles.uniMyRadius = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.radius");
    oglHandles.uniMyIsValud = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.isValid");
    oglHandles.uniMyIsLightSource = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.isLightSource");

    oglHandles.uniSunCenterTransformed = getUniformLocation(oglHandles.shaderProgram, "sunCenterTransformed");
    oglHandles.uniSunRadius = getUniformLocation(oglHandles.shaderProgram, "sunRadius");

    oglHandles.uniOtherSphereCenterTransformed = getUniformLocation(oglHandles.shaderProgram, "otherSphereCenterTransformed");
    oglHandles.uniOtherSphereRadius = getUniformLocation(oglHandles.shaderProgram, "otherSphereRadius");


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
        sizeof(float) * _core.axis.getVertices().size(),
        _core.axis.getVertices().data(),
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
        sizeof(float) * _core.starVertices.size(),
        _core.starVertices.data(),
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
        sizeof(float) * _core.gstarVertices.size(),
        _core.gstarVertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------



    sunRenderer.setAsLightSource();
    sunRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    sunRenderer.constructVerticesAndSendToGpu(oglHandles);


    earthRenderer.setPolygonCountLevel(PolygonCountLevel_High);
    earthRenderer.constructVerticesAndSendToGpu(oglHandles);
    earthRenderer.bShowLatitudesAndLongitudes = true;
    earthRenderer.setNightColorDarkness(NightColorDarkness_Medium);


    moonRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
    moonRenderer.constructVerticesAndSendToGpu(oglHandles);
    moonRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);


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


/*!
****************************************************************************
 The upper layers must use this method to tell the core of the change of the
 screen's width and height.

****************************************************************************/
void Renderer::seCurrenttScreenSize(int width, int height)
{
    curWidth = width;
    curHeight = height;
}

/*************************************************************************************************


**************************************************************************************************/
void Renderer::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
