#pragma once

#include "SphericalBody.h"
#include "GlslProgram.h"
#include <GL/glew.h>
#include "Renderer.h"

class CoordinateAxisRenderer : public Renderer
{
public:
    CoordinateAxisRenderer();
    ~CoordinateAxisRenderer();
    void init();

    void constructVerticesAndSendToGpu();
    void _renderAxis(GlslProgram glslProgram);
    virtual void renderMain(GlslProgram& glslProgram);

    void advance(float stepMultiplier) {}

    void setSpan(float xHalfSpan, float yHalfSpan, float zHalfSpan)
    {
        this->xHalfSpan = xHalfSpan;
        this->yHalfSpan = yHalfSpan;
        this->zHalfSpan = zHalfSpan;
    }

    void setColors(glm::vec3 xColor, glm::vec3 yColor, glm::vec3 zColor)
    {
        this->xColor = xColor;
        this->yColor = yColor;
        this->zColor = zColor;
    }

private:
    std::vector<float>* _constructVertices();

private:
    GLuint _axisVao = 0;
    int numVertices = 0;



private:
    glm::vec3 xColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 yColor = glm::vec3(1.0f, 1.0f, 1.0f);;
    glm::vec3 zColor = glm::vec3(1.0f, 1.0f, 1.0f);;

    float xHalfSpan = 1000.0f;
    float yHalfSpan = 1000.0f;
    float zHalfSpan = 1000.0f;

};