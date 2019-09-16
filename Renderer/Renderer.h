#pragma once

#include "SphereRenderer.h"
#include "UCore.h"


class Renderer
{
public:
    Renderer(Universe& core);
    ~Renderer();

    void render();
    void initializeGL();

    void printGlError();
    void printShaderCompileStatus(GLuint shader);
    void readAndCompileShader(std::string filePath, GLuint &shaderId);
    void compileShaders();
    void linkShaders();
    void useShaderProgram();
    void unuseShaderProgram();

    void seCurrenttScreenSize(int width, int height);
    void render();

public:
    Universe& _core;

    int curWidth = 0;
    int curHeight = 0;


    SphereRenderer earthRenderer;
    SphereRenderer sunRenderer;
    SphereRenderer moonRenderer;

    GLint uniOverrideColor;

    OglHandles oglHandles;
    GLuint vbo;                 // Create for 

};