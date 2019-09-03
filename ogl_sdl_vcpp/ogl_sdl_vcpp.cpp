// ogl_sdl_vcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <stdio.h>
//#pragma comment(lib,"glew32.lib")

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>


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
int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);


	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	//SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	printf("Created SDL GL window\n");
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewInit();

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	float vertices[] = {
	0.0f, 0.5f,
	0.5f, -0.5f,
	-0.5f, -0.5f
	};
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//-------------------------------------------------------------
	// Vertex shader
	const char* vertexShaderSource = R"glsl(
        #version 150 core

        in vec2 position;

        void main()
        {
            gl_Position = vec4(position, 0.0, 1.0);
        }

    )glsl";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

	printf("Compiling vertex shader.\n");
	glCompileShader(vertexShader);

	// Check compile status
	printShaderCompileStatus(vertexShader);


	//-------------------------------------------------------------
	// Fragment shader
	const char* fragmentShaderSource = R"glsl(
        #version 150 core

        out vec4 outColor;
        void main()
        {
            outColor = vec4(0.0, 1.0, 1.0, 1.0);
        }
    )glsl";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

	printf("Compiling fragment shader.\n");
	glCompileShader(fragmentShader);

	printShaderCompileStatus(fragmentShader);


	//-------------------------------------------------------------
	// Shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);


	// The following function is not available in GLSL 1.5, and is not required.
	//glBindFragDataLocation(shaderProgram, 0, "outColor");

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	//-------------------------------------------------------------
	// Make a link between vertex data and attributes
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(posAttrib);





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

		printf("Iterating\n");
		glClearDepth(1.0);
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//glEnableClientState(GL_COLOR_ARRAY);
		//glBegin(GL_TRIANGLES);
		//glColor3f(1.0f, 0.0f, 0.0f);
		//glVertex2f(0, 0);
		//glVertex2f(0, 500);
		//glVertex2f(500, 500);
		//glEnd();

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
