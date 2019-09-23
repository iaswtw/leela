#include "pch.h"
#include "AxisRenderer.h"

static inline void vector_push_back_7(std::vector<float>& v, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    v.push_back(f1);
    v.push_back(f2);
    v.push_back(f3);
    v.push_back(f4);
    v.push_back(f5);
    v.push_back(f6);
    v.push_back(f7);
}

AxisRenderer::AxisRenderer(Axis& axis)
    : _axis(axis)
{

}

AxisRenderer::~AxisRenderer()
{

}

std::vector<float>* AxisRenderer::_constructVertices()
{
    std::vector<float>* v = new std::vector<float>();

    float alpha = 0.8f;

    vector_push_back_7(*v, -_axis.xHalfSpan, 0.0f, 0.0f, _axis.xColor.r, _axis.xColor.g, _axis.xColor.b, alpha);
    vector_push_back_7(*v, +_axis.xHalfSpan, 0.0f, 0.0f, _axis.xColor.r, _axis.xColor.g, _axis.xColor.b, alpha);
    vector_push_back_7(*v, 0.0f, -_axis.yHalfSpan, 0.0f, _axis.yColor.r, _axis.yColor.g, _axis.yColor.b, alpha);
    vector_push_back_7(*v, 0.0f, +_axis.yHalfSpan, 0.0f, _axis.yColor.r, _axis.yColor.g, _axis.yColor.b, alpha);
    vector_push_back_7(*v, 0.0f, 0.0f, -_axis.zHalfSpan, _axis.zColor.r, _axis.zColor.g, _axis.zColor.b, alpha);
    vector_push_back_7(*v, 0.0f, 0.0f, +_axis.zHalfSpan, _axis.zColor.r, _axis.zColor.g, _axis.zColor.b, alpha);

    return v;
}

void AxisRenderer::constructVerticesAndSendToGpu()
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

void AxisRenderer::render(GlslProgram& glslProgram)
{
    //----------------------------------------------
    // Axis model transformation
    //----------------------------------------------
    glslProgram.setMat4("model", glm::value_ptr(glm::mat4(1.0)));
    glBindVertexArray(_axisVao);
    // Draw vertices
    glDrawArrays(GL_LINES, 0, numVertices);
}
