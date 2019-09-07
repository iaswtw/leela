// ogl_sdl_vcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <stdio.h>

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


float angle;

int curWidth;
int curHeight;

GLuint vbo;                 // Create for 
GLuint sunVao;
GLuint earthVao;
GLuint moonVao;
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
GLint uniSunCenter;
GLint uniMyCenter;
GLint uniMyRadius;
GLint uniMyIsValud;
GLint uniMyIsLightSource;

GLint _numAttributes;


Sphere earth;
Sphere sun;
Sphere moon;


void initSceneObjects()
{
    // Sun
    //---------------------------------------
    sun.setColor(0.7f, 0.7f, 0.1f);
    sun.setRotationParameters(60,                     // radius
        0,                      // initial rotation angle
        0.02f,                   // rotation velocity
        glm::radians(0.0f),     // axis rotation angle
        glm::radians(0.0f)      // axis tilt angle
    );
    sun.setOrbitalParameters(0,                       // radius of orbit
        0.01f,                       // revolution velocity
        0,                       // orbital rotation angle
        0                        // orbital tilt
    );


    // Earth
    //---------------------------------------
    earth.setColor(0.55f, 0.82f, 1.0f);
    earth.setRotationParameters(40,                     // radius
        0,                      // initial rotation angle
        0.05f,                   // rotation velocity
        glm::radians(270.0f),   // axis rotation angle
        glm::radians(23.5f)     // axis tilt angle
    );
    earth.setOrbitalParameters(400,                     // radius of orbit
        0.01f,                    // revolution velocity
        0,                       // orbital rotation angle
        0                        // orbital tilt
    );

    // Moon
    //---------------------------------------
    moon.setColor(0.5f, 0.5f, 0.5f);
    sun.setRotationParameters(15,                     // radius
        0,                      // initial rotation angle
        0.05f,                   // rotation velocity
        glm::radians(0.0f),     // axis rotation angle
        glm::radians(10.0f)     // axis tilt angle
    );
    sun.setOrbitalParameters(90,                      // radius of orbit
        0.04f,                    // revolution velocity
        0,                       // orbital rotation angle
        0                        // orbital tilt
    );

    earth.generateVertices();
    sun.generateVertices();
    //moon.generateVertices();
}

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
	}
}


void compileShaders()
{
	//-------------------------------------------------------------
	// Vertex shader
	//-------------------------------------------------------------
	const char* vertexShaderSource = R"glsl(
        #version 150 core

        in vec3 position;
        in vec3 in_color;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;

        //uniform vec3 sunCenter;

        //struct SphereInfo {
        //    bool isLightSource;
        //    bool isValid;
        //    vec3 center;
        //    float radius;
        //};

        //uniform SphereInfo sphereInfo;

        out vec4 Color;

        void main()
        {
            //if (sphereInfo.isLightSource == false)
            //{
            //    if (sphereInfo.isValid)
            //    {
            //        vec4 modelTransformedMyCenter = model * vec4(sphereInfo.center, 1.0);
            //        float r = length(vec2(length(modelTransformedMyCenter),
            //                              sphereInfo.radius));


            //        vec4 modelTransformedPosition = model * vec4(position, 1.0);

            //        if (r < length(modelTransformedPosition))
            //            Color = vec4(color*0.2, 1.0);
            //        else
            //            Color = vec4(color, 1.0);
            //    }
            //}
            //else
            //{
            //    Color = vec4(color, 0.5);
            //}
            
            Color = vec4(in_color, 1.0);

            gl_Position = proj * view * model * vec4(position, 1.0);
        }

    )glsl";

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

	printf("Compiling vertex shader.\n");
	glCompileShader(vertexShader);

	// Check compile status
	printShaderCompileStatus(vertexShader);


	//-------------------------------------------------------------
	// Fragment shader
	//-------------------------------------------------------------
	const char* fragmentShaderSource = R"glsl(
        #version 150 core

        in vec4 Color;

        out vec4 outColor;

        void main()
        {
            // outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            outColor = Color;
        }
    )glsl";

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

	printf("Compiling fragment shader.\n");
	glCompileShader(fragmentShader);

	printShaderCompileStatus(fragmentShader);

	//-------------------------------------------------------------
	// Shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);


	// The following function is not available in GLSL 1.5, and is not required.
//    glBindFragDataLocation(shaderProgram, 0, "outColor");

}

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


void useShaderProgram()
{
	glUseProgram(shaderProgram);
}

void unuseShaderProgram()
{
	glUseProgram(0);
}



void initializeGL()
{
	printf("Inside initializeGL\n");

	compileShaders();

    glBindAttribLocation(shaderProgram, 0, "position");
    glBindAttribLocation(shaderProgram, 1, "in_color");

	linkShaders();


    //---------------------------------------------------------------------------------------------------
    uniModel = glGetUniformLocation(shaderProgram, "model");
    uniView = glGetUniformLocation(shaderProgram, "view");
    uniProj = glGetUniformLocation(shaderProgram, "proj");
    //uniSunCenter = glGetUniformLocation(shaderProgram, "sunCenter");
    //uniMyCenter = glGetUniformLocation(shaderProgram, "sphereInfo.center");
    //uniMyRadius = glGetUniformLocation(shaderProgram, "sphereInfo.radius");
    //uniMyIsValud = glGetUniformLocation(shaderProgram, "sphereInfo.isValid");
    //uniMyIsLightSource = glGetUniformLocation(shaderProgram, "sphereInfo.isValid");


    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    GLint colAttrib = glGetAttribLocation(shaderProgram, "in_color");

    printf("uniModel = %d\n", uniModel);
    printf("uniView = %d\n", uniView);
    printf("uniProj = %d\n", uniProj);

    //printf("uniSunCenter = %d\n", uniSunCenter);
    //printf("uniMyCenter = %d\n", uniMyCenter);
    //printf("uniMyRadius = %d\n", uniMyRadius);
    //printf("uniMyIsValud = %d\n", uniMyIsValud);
    //printf("uniMyIsLightSource = %d\n", uniMyIsLightSource);


    printf("posAttrib = %d\n", posAttrib);
    printf("colAttrib = %d\n", colAttrib);  // prints -1


    //---------------------------------------------------------------------------------------------------
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
    printGlError();     // this is printing error because colAttrib is -1
    glEnableVertexAttribArray(colAttrib);

    //---------------------------------------------------------------------------------------------------
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
    glBindVertexArray(0);       // Disable VBO

}



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

	SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	printf("Created SDL GL window\n");
	SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

	glewInit();

	
	//float vertices[] = {
	//0.0f, 0.5f,
	//0.5f, -0.5f,
	//-0.5f, -0.5f
	//};
	//GLuint vbo;
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	////-------------------------------------------------------------
	//// Vertex shader
	//const char* vertexShaderSource = R"glsl(
 //       #version 150 core

 //       in vec2 position;

 //       void main()
 //       {
 //           gl_Position = vec4(position, 0.0, 1.0);
 //       }

 //   )glsl";

	//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

	//printf("Compiling vertex shader.\n");
	//glCompileShader(vertexShader);

	//// Check compile status
	//printShaderCompileStatus(vertexShader);


	////-------------------------------------------------------------
	//// Fragment shader
	//const char* fragmentShaderSource = R"glsl(
 //       #version 150 core

 //       out vec4 outColor;
 //       void main()
 //       {
 //           outColor = vec4(0.0, 1.0, 1.0, 1.0);
 //       }
 //   )glsl";

	//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

	//printf("Compiling fragment shader.\n");
	//glCompileShader(fragmentShader);

	//printShaderCompileStatus(fragmentShader);


	////-------------------------------------------------------------
	//// Shader program
	//GLuint shaderProgram = glCreateProgram();
	//glAttachShader(shaderProgram, vertexShader);
	//glAttachShader(shaderProgram, fragmentShader);


	//// The following function is not available in GLSL 1.5, and is not required.
	////glBindFragDataLocation(shaderProgram, 0, "outColor");

	//glLinkProgram(shaderProgram);
	//glUseProgram(shaderProgram);

	////-------------------------------------------------------------
	//// Make a link between vertex data and attributes
	//GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	//glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//glEnableVertexAttribArray(posAttrib);


    printf("initializing scene objects... ");
    initSceneObjects();
	initializeGL();

    printf("done\n");

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    curWidth = DM.w;
    curHeight = DM.h;

    glEnable(GL_DEPTH_TEST);


	SDL_Event windowEvent;
	while (1)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT)
				break;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
				break;

		}

        //printf("Iterating\n");
        
        //glClearDepth(1.0);
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        earth.advance(1);
        sun.advance(1);

        useShaderProgram();



        // todo - this could be moved to initialization?
        glUniform3f(uniSunCenter, sun.getCenter().x, sun.getCenter().y, sun.getCenter().z);

        //=====================================================================================
        // View and projection remain same for the entire scene
        //=====================================================================================
        // View transformation
        //----------------------------------------------
        glm::mat4 view = glm::lookAt(
            glm::vec3(900.2f, 900.2f, 900.2f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        // perspective transformation
        //----------------------------------------------
        glm::mat4 proj = glm::perspective(glm::radians(35.0f),
            float(curWidth) / float(curHeight),
            1.0f,
            10000.0f);
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        //=====================================================================================


        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        //----------------------------------------------
        // earth model transformation
        //----------------------------------------------
        glm::mat4 trans(1.0f);
        trans = glm::translate(trans, glm::vec3(earth.getCenter().x,
            earth.getCenter().y,
            earth.getCenter().z));
        trans = glm::rotate(trans, angle, glm::vec3(0.0f,
            0.0f,
            earth.getAxisRotationAngle()));
        trans = glm::rotate(trans, angle, glm::vec3(0.0f,
            earth.getAxisTiltAngle(),
            0.0f));
        trans = glm::rotate(trans, angle, glm::vec3(0.0f,
            0.0f,
            earth.getRotationAngle()));

        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans));

        //glUniform1i(uniMyIsLightSource, false);
        //glUniform3f(uniMyCenter, 0.0f, 0.0f, 0.0f);
        //glUniform1f(uniMyRadius, earth.getRadius());
        //glUniform1i(uniMyIsValud, true);

        glBindVertexArray(earthVao);
        
        // Draw vertices
        glDrawArrays(GL_TRIANGLES, 0, earth.getVertices().size());
        

        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        //----------------------------------------------
        // sun model transformation
        //----------------------------------------------
        glm::mat4 trans2(1.0f);

        trans2 = glm::rotate(trans2, angle, glm::vec3(0.0f,
            0.0f,
            sun.getRotationAngle()));

        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans2));

        //glUniform1i(uniMyIsLightSource, false);
        //glUniform1f(uniMyRadius, sun.getRadius());
        //glUniform3f(uniMyCenter, 0.0f, 0.0f, 0.0f);
        //glUniform1i(uniMyIsValud, true);

        glBindVertexArray(sunVao);

        glDrawArrays(GL_TRIANGLES, 0, sun.getVertices().size());

        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        glBindVertexArray(0);

        unuseShaderProgram();

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
