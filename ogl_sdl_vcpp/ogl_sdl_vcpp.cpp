// ogl_sdl_vcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <stdio.h>

#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"
#include "Space.h"

#include <fstream>
#include <iostream>
#include <string>

float angle;

int curWidth;
int curHeight;

GLuint vbo;                 // Create for 
GLuint axisVao;
GLuint earthOrbitalPlaneVao;
GLuint sunVao;
GLuint earthVao;
GLuint moonVao;



GLuint earthLatLongVao;

GLuint shaderProgram;
GLuint vertexShader;
GLuint fragmentShader;

// Texture handles
GLuint tex1;
GLuint tex2;

GLint uniColor;
GLint uniModel;
GLint uniView;
GLint uniProj;

GLint uniOverrideColor;

GLint uniMyCenterTransformed;
//GLint uniMyRadius;
GLint uniMyIsValud;
GLint uniMyIsLightSource;

GLint uniOtherSphereCenterTransformed;
GLint uniOtherSphereRadius;

GLint uniSunCenterTransformed;
GLint uniSunRadius;


GLint _numAttributes;


glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;


// Sphere and other objects to be drawn on the screen. Instantiate them here. Their data (vertices) will be created later.
Axis axis;
Sphere earth;
Sphere sun;
Sphere moon;


Space space;

/*************************************************************************************************


**************************************************************************************************/
void initSceneObjects()
{
    space.initFrame();


    axis.generateVertices(
        1400, 1400, 800,
        glm::vec3(0.2f, 0.2f, 0.5f),        // X axis color
        glm::vec3(0.2f, 0.5f, 0.2f),        // Y axis color
        glm::vec3(0.2f, 0.5f, 0.5f)         // Z axis color
    );
    // Sun
    //---------------------------------------
    sun.setColor(0.7f, 0.7f, 0.1f);
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
    earth.setOrbitalParameters(1000,         // radius of orbit
        glm::radians(240.0f),               // initial orbital angle
        0.001f,                             // revolution velocity
        0.0f,                               // orbital rotation angle
        0                                   // orbital tilt
    );
     earth.enableOrbitalPlaneGeneration();

    // Moon
    //---------------------------------------
    moon.setColor(0.5f, 0.5f, 0.5f);
    moon.setRotationParameters(30,          // radius
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


    earth.generateVertices();
    sun.generateVertices();
    moon.generateVertices();

    moon.setParentSphere(&earth);
    earth.setParentSphere(&sun);
}

/*************************************************************************************************


**************************************************************************************************/
void printGlError()
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
void printShaderCompileStatus(GLuint shader)
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
void readAndCompileShader(std::string filePath, GLuint &shaderId)
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
void compileShaders()
{
	//-------------------------------------------------------------
	// Vertex shader
	//-------------------------------------------------------------
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    printf("Loading and Compiling Vertex shader.\n");
    readAndCompileShader("shaders/shader.vert", vertexShader);

    //-------------------------------------------------------------
	// Fragment shader
	//-------------------------------------------------------------
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    printf("Loading and Compiling Fragment shader.\n");
    readAndCompileShader("shaders/shader.frag", fragmentShader);

	//-------------------------------------------------------------
	// Shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
}

/*************************************************************************************************


**************************************************************************************************/
void linkShaders()
{
	glLinkProgram(shaderProgram);

	// todo perform check to see if linking was successful
	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

		printf("Linker error log:\n");
		printf("%s\n", infoLog.data());

		// We don't need the program anymore.
		glDeleteProgram(shaderProgram);
	}
	else
	{
		printf("\nShader program link successful\n");
		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);
	}

    // Don't leak shaders either.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}


/*************************************************************************************************


**************************************************************************************************/
void useShaderProgram()
{
	glUseProgram(shaderProgram);
}

/*************************************************************************************************


**************************************************************************************************/
void unuseShaderProgram()
{
	glUseProgram(0);
}

/*************************************************************************************************


**************************************************************************************************/
GLint getUniformLocation(const std::string& uniformName)
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
GLint getAttribLocation(const std::string& attribName)
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

/*************************************************************************************************


**************************************************************************************************/
void initializeGL()
{
	printf("Inside initializeGL\n");

	compileShaders();

    glBindAttribLocation(shaderProgram, 0, "position");
    glBindAttribLocation(shaderProgram, 1, "in_color");

	linkShaders();


    //---------------------------------------------------------------------------------------------------
    uniModel                        = getUniformLocation("model");
    uniView                         = getUniformLocation("view");
    uniProj                         = getUniformLocation("proj");
    
    uniMyCenterTransformed          = getUniformLocation("sphereInfo.centerTransformed");
    //uniMyRadius                     = getUniformLocation("sphereInfo.radius");
    uniMyIsValud                    = getUniformLocation("sphereInfo.isValid");
    uniMyIsLightSource              = getUniformLocation("sphereInfo.isLightSource");

    uniSunCenterTransformed         = getUniformLocation("sunCenterTransformed");
    uniSunRadius                    = getUniformLocation("sunRadius");

    uniOtherSphereCenterTransformed = getUniformLocation("otherSphereCenterTransformed");
    uniOtherSphereRadius            = getUniformLocation("otherSphereRadius");


    GLint posAttrib                 = getAttribLocation("position");
    GLint colAttrib                 = getAttribLocation("in_color");
    


    //---------------------------------------------------------------------------------------------------
    // Axis
    glGenVertexArrays(1, &axisVao);
    glBindVertexArray(axisVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * axis.getVertices().size(),
        axis.getVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);


    //---------------------------------------------------------------------------------------------------
    // Earth's sphere
    glGenVertexArrays(1, &earthVao);
    glBindVertexArray(earthVao);

    glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * earth.getVertices().size(),
        earth.getVertices().data(),
		GL_STATIC_DRAW);

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Earths latitude and longitude
    glGenVertexArrays(1, &earthLatLongVao);
    glBindVertexArray(earthLatLongVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * earth.getLatitudeAndLongitudeVertices().size(),
        earth.getLatitudeAndLongitudeVertices().data(),
        GL_STATIC_DRAW);
    
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Earth's orbital plane
    glGenVertexArrays(1, &earthOrbitalPlaneVao);
    glBindVertexArray(earthOrbitalPlaneVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * earth.getOrbitalPlaneVertices().size(),
        earth.getOrbitalPlaneVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Sun's sphere
    glGenVertexArrays(1, &sunVao);
    glBindVertexArray(sunVao);
    
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
        GL_ARRAY_BUFFER,
		sizeof(float) * sun.getVertices().size(),
		sun.getVertices().data(),
		GL_STATIC_DRAW);

	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Moon's sphere
    glGenVertexArrays(1, &moonVao);
    glBindVertexArray(moonVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * moon.getVertices().size(),
        moon.getVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);
    
    //---------------------------------------------------------------------------------------------------
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


/*************************************************************************************************


**************************************************************************************************/
void render()
{
    //glClearDepth(1.0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    useShaderProgram();

    glUniform3fv(
        uniSunCenterTransformed,
        1,
        glm::value_ptr(sun.getModelTransformedCenter())
    );

    glUniform1f(uniSunRadius, sun.getRadius());

    //std::cout << glm::to_string(space.getDirectionPoint()) << std::endl;
    //std::cout << glm::to_string(space.getSourcePoint()) << std::endl;
    //std::cout << glm::to_string(space.getUpwardDirectionVector()) << std::endl;

    //std::cout << endl;

    //=====================================================================================
    // View and projection remain same for the entire scene
    //=====================================================================================
    // View transformation
    //----------------------------------------------
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

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
        10000.0f);

    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    //=====================================================================================

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //----------------------------------------------
    // Axis model transformation
    //----------------------------------------------
    glUniformMatrix4fv(
        uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(glm::mat4(1.0))
    );

    glUniform1i(uniMyIsValud, false);
    // ideally, after setting IsValid to false, no need to set the other variables to draw the axis.
    glUniform1i(uniMyIsLightSource, 0);
    glUniform3f(uniMyCenterTransformed, 0.0f, 0.0f, 0.0f);
    //glUniform1f(uniMyRadius, 0);

    // When drawing earth, other sphere is moon.
    glUniform1f(uniOtherSphereRadius, 0);
    glUniform3fv(
        uniOtherSphereCenterTransformed,
        1,
        glm::value_ptr(moon.getModelTransformedCenter())
    );

    glBindVertexArray(axisVao);

    // Draw vertices
    glDrawArrays(GL_LINES, 0, axis.getVertices().size() / 7);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    //----------------------------------------------
    // earth model transformation
    //----------------------------------------------
    glUniformMatrix4fv(
        uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(earth.getModelMatrix())
    );

    glUniform1i(uniMyIsLightSource, 0);
    glUniform3fv(uniMyCenterTransformed, 1, glm::value_ptr(earth.getCenter()));
    //glUniform1f(uniMyRadius, earth.getRadius());
    glUniform1i(uniMyIsValud, true);

    // When drawing earth, other sphere is moon.
    glUniform1f(uniOtherSphereRadius, moon.getRadius());
    glUniform3fv(
        uniOtherSphereCenterTransformed,
        1,
        glm::value_ptr(moon.getModelTransformedCenter())
    );

    glBindVertexArray(earthVao);

    // Draw vertices
    glDrawArrays(GL_TRIANGLES, 0, earth.getVertices().size() / 7);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    glBindVertexArray(earthLatLongVao);

    // Draw vertices
    glDrawArrays(GL_LINES, 0, earth.getLatitudeAndLongitudeVertices().size() / 7);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    glUniformMatrix4fv(
        uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(earth.getOrbitalPlaneModelMatrix())
    );

    glUniform1i(uniMyIsValud, false);

    glBindVertexArray(earthOrbitalPlaneVao);

    // Draw vertices
    glDrawArrays(GL_TRIANGLES, 0, earth.getOrbitalPlaneVertices().size() / 7);


    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    //----------------------------------------------
    // sun model transformation
    //----------------------------------------------
    glUniformMatrix4fv(
        uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(sun.getModelMatrix())
    );

    glUniform1i(uniMyIsLightSource, 1);
    //glUniform1f(uniMyRadius, sun.getRadius());
    glUniform3fv(uniMyCenterTransformed, 1, glm::value_ptr(sun.getCenter()));
    glUniform1i(uniMyIsValud, true);

    glBindVertexArray(sunVao);

    glDrawArrays(GL_TRIANGLES, 0, sun.getVertices().size() / 7);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    //----------------------------------------------
    // moon model transformation
    //----------------------------------------------
    glUniformMatrix4fv(
        uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(moon.getModelMatrix())
    );

    glUniform1i(uniMyIsLightSource, 0);
    glUniform3fv(uniMyCenterTransformed, 1, glm::value_ptr(moon.getCenter()));
    //glUniform1f(uniMyRadius, moon.getRadius());
    glUniform1i(uniMyIsValud, true);

    // When drawing moon, other sphere is earth.
    glUniform1f(uniOtherSphereRadius, earth.getRadius());
    glUniform3fv(
        uniOtherSphereCenterTransformed,
        1,
        glm::value_ptr(earth.getModelTransformedCenter())
    );

    glBindVertexArray(moonVao);

    // Draw vertices
    glDrawArrays(GL_TRIANGLES, 0, moon.getVertices().size() / 7);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    glBindVertexArray(0);

    unuseShaderProgram();

}




bool bQuit = false;
bool bMouseGrabbed = false;
bool bFastForward = false;
bool bFastReverse = false;

bool bLeftMouseButtonDown = false;
bool bRightMouseButtonDown = false;
int dx = 0;
int dy = 0;
int previousX = 0;
int previousY = 0;
float stepMultiplier = 1;
float previousStepMultiplier = stepMultiplier;


char F_L_BUTTON_DOWN = 0;
char F_R_BUTTON_DOWN = 0;
char F_M_BUTTON_DOWN = 0;

bool bSidewaysMotionMode = true;
bool bLockOntoEarth = false;
bool bLockOntoSun = false;

/*! \todo F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
    or bLockOntoSun in the function on_MouseMotion().  Need
    to consider if the priority of this check should be reversed.
    Without the setting this flag to 1, the Lock on earth or sun won't
    work. */
char F_REFERENCE_VECTOR_ALONG_Z = 0;

bool bExplicitMouseMove = false;

void onMouseMotion(int dx, int dy);

/*************************************************************************************************


**************************************************************************************************/
int main(int argc, char *argv[])
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
                switch (event.key.keysym.sym) {
                case SDLK_f:
                    if (!bFastForward) {
                        previousStepMultiplier = stepMultiplier;
                        stepMultiplier *= 5;
                        bFastForward = true;
                    }
                    break;
                case SDLK_r:
                    if (!bFastReverse) {
                        previousStepMultiplier = stepMultiplier;
                        stepMultiplier *= -5;
                        bFastReverse = true;
                    }
                    break;
                case SDLK_UP:
                    stepMultiplier *= 1.25;
                    break;
                case SDLK_DOWN:
                    stepMultiplier *= 0.8;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    bMouseGrabbed = false;
                    SDL_ShowCursor(SDL_ENABLE);
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    break;
                case SDLK_f:         bFastForward = false;  stepMultiplier = previousStepMultiplier;        break;
                case SDLK_r:         bFastReverse = false;  stepMultiplier = previousStepMultiplier;        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (!bMouseGrabbed) {
                    bMouseGrabbed = true;
                    SDL_ShowCursor(SDL_DISABLE);
                    //SDL_WarpMouseInWindow(window, curWidth / 2, curHeight / 2);
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    bExplicitMouseMove = true;
                }
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:   bLeftMouseButtonDown = true;   F_L_BUTTON_DOWN = 1;   break;
                case SDL_BUTTON_RIGHT:  bRightMouseButtonDown = true;  F_R_BUTTON_DOWN = 1;   break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:   bLeftMouseButtonDown = false;   F_L_BUTTON_DOWN = 0;      break;
                case SDL_BUTTON_RIGHT:  bRightMouseButtonDown = false;  F_R_BUTTON_DOWN = 0;      break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (bMouseGrabbed)
                {
                    //if (bExplicitMouseMove) {
                    //    bExplicitMouseMove = false;
                    //    break;
                    //}

                    //dx = event.motion.x - previousX;
                    //dy = event.motion.y - previousY;
                    
                    //previousX = event.motion.x;
                    //previousY = event.motion.y;

                    //SDL_WarpMouseInWindow(window, curWidth / 2, curHeight / 2);
                    //bExplicitMouseMove = true;
                    onMouseMotion(event.motion.xrel, event.motion.yrel);
                    //onMouseMotion(0, 10);
                    //printf("Mouse moved by: %d, %d\n", dx, dy);
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
        
        // Advance parents before children
        sun.advance(stepMultiplier);
        earth.advance(stepMultiplier);
        moon.advance(stepMultiplier);

        render();

		SDL_GL_SwapWindow(window);
		SDL_Delay(10);
	}

	SDL_Quit();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


void onMouseMotion(int dx, int dy)
{
    //printf("dx = %d, dy = %d\n", dx, dy);

    if (F_REFERENCE_VECTOR_ALONG_Z == 1)
    {
        if (F_L_BUTTON_DOWN == 1) {
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
        if (F_L_BUTTON_DOWN == 1) {
            space.moveFrame(Movement_Forward, -dy * 5);


            if (bSidewaysMotionMode == 1) {
                space.moveFrame(Movement_RotateRight, 90);
                space.moveFrame(Movement_Forward, double(dx));
                space.moveFrame(Movement_RotateLeft, 90);
            }
            else {
                space.moveFrame(Movement_RotateRight, dx / 10.);
            }
            break;
        }

        if (F_R_BUTTON_DOWN == 1) {
            space.moveFrame(Movement_RightAlongSD, double(dx / 10));
            break;
        }

        if (bSidewaysMotionMode) {
            space.moveFrame(Movement_RotateRight, 90);
            space.moveFrame(Movement_Forward, double(dx));
            space.moveFrame(Movement_RotateLeft, 90);

            space.moveFrame(Movement_RotateUp, 90);
            space.moveFrame(Movement_Forward, -double(dy));
            space.moveFrame(Movement_RotateDown, 90);

        }
        else {
            space.moveFrame(Movement_RotateRight, double(dx / 10));
            space.moveFrame(Movement_RotateUp, -double(dy / 10));
        }
        break;
    }
}

