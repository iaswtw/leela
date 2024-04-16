#include "Utils.h"
#include "CoordinateAxisRenderer.h"
#include "Universe.h"


CoordinateAxisRenderer::CoordinateAxisRenderer()
{
}

CoordinateAxisRenderer::~CoordinateAxisRenderer()
{
}



void CoordinateAxisRenderer::init()
{
    constructVerticesAndSendToGpu();
}

std::vector<float>* CoordinateAxisRenderer::_constructVertices()
{
    std::vector<float>* v = new std::vector<float>();

    float alpha = 0.8f;

    vector_push_back_7(*v, -xHalfSpan, 0.0f, 0.0f, xColor.r, xColor.g, xColor.b, alpha);
    vector_push_back_7(*v, +xHalfSpan, 0.0f, 0.0f, xColor.r, xColor.g, xColor.b, alpha);
    vector_push_back_7(*v, 0.0f, -yHalfSpan, 0.0f, yColor.r, yColor.g, yColor.b, alpha);
    vector_push_back_7(*v, 0.0f, +yHalfSpan, 0.0f, yColor.r, yColor.g, yColor.b, alpha);
    vector_push_back_7(*v, 0.0f, 0.0f, -zHalfSpan, zColor.r, zColor.g, zColor.b, alpha);
    vector_push_back_7(*v, 0.0f, 0.0f, +zHalfSpan, zColor.r, zColor.g, zColor.b, alpha);

    return v;
}

void CoordinateAxisRenderer::constructVerticesAndSendToGpu()
{
    GLuint vbo;
    std::vector<float>* v = nullptr;
    v = _constructVertices();

    glGenVertexArrays(1, &_axisVao);
    glBindVertexArray(_axisVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;

}


void CoordinateAxisRenderer::_renderAxis(GlslProgram glslProgram)
{
    if (g_universe->bShowAxis) {
        //----------------------------------------------
        // Axis model transformation
        //----------------------------------------------
        glslProgram.setMat4("model", glm::value_ptr(glm::mat4(1.0)));
        glBindVertexArray(_axisVao);
        // Draw vertices
        glDrawArrays(GL_LINES, 0, numVertices);
    }
}



void CoordinateAxisRenderer::renderMain(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_Simple)
    {
        _renderAxis(glslProgram);
    }
}
