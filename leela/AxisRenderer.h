#pragma once

#include "Sphere.h"
#include "GlslProgram.h"
#include <GL/glew.h>

class AxisRenderer
{
public:
    AxisRenderer(Axis& axis);
    ~AxisRenderer();

    void constructVerticesAndSendToGpu();
    void render(GlslProgram& glslProgram);

private:
    std::vector<float>* _constructVertices();

private:
    GLuint _axisVao = 0;
    int numVertices = 0;

    Axis& _axis;
};