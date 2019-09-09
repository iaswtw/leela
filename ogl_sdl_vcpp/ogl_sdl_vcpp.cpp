// ogl_sdl_vcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <stdio.h>

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"

#include <fstream>
#include <iostream>
#include <string>

float angle;

int curWidth;
int curHeight;

GLuint vbo;                 // Create for 
GLuint axisVao;
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


// Sphere and other objects to be drawn on the screen. Instantiate them here. Their data (vertices) will be created later.
Axis axis;
Sphere earth;
Sphere sun;
Sphere moon;


/*************************************************************************************************


**************************************************************************************************/
void initSceneObjects()
{
    axis.generateVertices(
        1400,
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
        0.0001f,                             // revolution velocity
        0.0f,                               // orbital rotation angle
        0                                   // orbital tilt
    );

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
        0.004f,                             // revolution velocity
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

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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
    
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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

	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);
    
    //---------------------------------------------------------------------------------------------------
    glBindVertexArray(0);       // Disable VBO

}


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

    //=====================================================================================
    // View and projection remain same for the entire scene
    //=====================================================================================
    // View transformation
    //----------------------------------------------
    glm::mat4 view = glm::lookAt(
        //            glm::vec3(1600.0f, 1600.0f, 1600.0f),
        glm::vec3(-200.0f, -700.0f, 200.0f),
        //glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-650.0f, -900.0f, -100.0f),
        glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    // perspective transformation
    //----------------------------------------------
    glm::mat4 proj = glm::perspective(
        glm::radians(35.0f),
        float(curWidth) / float(curHeight),
        1.0f,
        10000.0f);
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
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
    glDrawArrays(GL_LINES, 0, axis.getVertices().size() / 6);

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
    glDrawArrays(GL_TRIANGLES, 0, earth.getVertices().size() / 6);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    glBindVertexArray(earthLatLongVao);

    // Draw vertices
    glDrawArrays(GL_LINES, 0, earth.getLatitudeAndLongitudeVertices().size() / 6);


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

    glDrawArrays(GL_TRIANGLES, 0, sun.getVertices().size() / 6);

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
    glDrawArrays(GL_TRIANGLES, 0, moon.getVertices().size() / 6);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    glBindVertexArray(0);

    unuseShaderProgram();

}

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

	SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	printf("Created SDL GL window\n");
	SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

	glewInit();

    printf("initializing scene objects... ");
    initSceneObjects();
	initializeGL();


    printf("done\n");

    SDL_GetWindowSize(window, &curWidth, &curHeight);
    printf("width = %d\n", curWidth);
    printf("height = %d\n", curHeight);

    glEnable(GL_DEPTH_TEST);

    bool bQuit = false;
    bool bFastForward = false;
    bool bFastReverse = false;

	SDL_Event event;
	while (1)
	{
		if (SDL_PollEvent(&event))
		{
            switch (event.type)
            {
            case SDL_QUIT:
                bQuit = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_f:
                    bFastForward = true;
                    break;
                case SDLK_r:
                    bFastReverse = true;
                    break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    bQuit = true;
                    break;
                case SDLK_f:
                    bFastForward = false;
                    break;
                case SDLK_r:
                    bFastReverse = false;
                    break;
                }
                break;

            case SDL_WINDOWEVENT:
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    curWidth = event.window.data1;
                    curHeight = event.window.data2;

                    printf("width = %d\n", curWidth);
                    printf("height = %d\n", curHeight);
                    glViewport(0, 0, curWidth, curHeight);
                }
                break;
            }
            }

            if (bQuit)
                break;
		}

        //printf("Iterating\n");
        
        float stepMultiplier = 0.05;
        if (bFastForward)
            stepMultiplier = 1;
        else if (bFastReverse)
            stepMultiplier = -1;

        // Advance parents before children
        sun.advance(stepMultiplier);
        earth.advance(stepMultiplier);
        moon.advance(stepMultiplier);

        render();

		SDL_GL_SwapWindow(window);
		SDL_Delay(0);
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
