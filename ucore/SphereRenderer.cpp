#include "pch.h"
#include "SphereRenderer.h"
#include "Utils.h"


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

SphereRenderer::SphereRenderer(Sphere& sphere) :
    _sphere(sphere)
{
    setNightColorDarkness(NightColorDarkness_High);
}


SphereRenderer::~SphereRenderer()
{
}

void SphereRenderer::setAsLightSource()
{
    _bIsLightSource = True;
}

void SphereRenderer::setNightColorDarkness(NightColorDarkness darkness)
{
    _nightColorDarkness = darkness;
    switch (_nightColorDarkness)
    {
    case NightColorDarkness_Black:      _nightColorMultiplier = 0.0f;   break;
    case NightColorDarkness_VeryHigh:   _nightColorMultiplier = 0.05f;  break;
    case NightColorDarkness_High:       _nightColorMultiplier = 0.15f;  break;
    case NightColorDarkness_Medium:     _nightColorMultiplier = 0.2f;   break;
    case NightColorDarkness_Low:        _nightColorMultiplier = 0.5f;   break;
    case NightColorDarkness_VeryLow:    _nightColorMultiplier = 0.8f;   break;
    case NightColorDarkness_None:       _nightColorMultiplier = 1.0f;   break;
    }
}

void SphereRenderer::setPolygonCountLevel(PolygonCountLevel polygonCountLevel)
{
	_polygonCountLevel = polygonCountLevel;
}


std::vector<float>* SphereRenderer::_constructMainSphereVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

	float polygonIncrement = _getPolygonIncrement();

	float alpha_inc = float(2 * M_PI) / polygonIncrement;
    float theat_inc = float(M_PI) / (polygonIncrement / 2);

    int numFloats = int((2 * M_PI / alpha_inc) * (M_PI / theat_inc)) * 7;
    printf("numFloats = %d\n", numFloats);

    v->reserve(numFloats);

    for (float alpha = 0; alpha < float(2 * M_PI); alpha += alpha_inc)
    {
        for (float theta = 0; theta < float(M_PI); theta += theat_inc)
        {
            float x1 = s._radius * sin(theta) * cos(alpha);
            float y1 = s._radius * sin(theta) * sin(alpha);
            float z1 = s._radius * cos(theta);

            float x2 = s._radius * sin(theta + theat_inc) * cos(alpha);
            float y2 = s._radius * sin(theta + theat_inc) * sin(alpha);
            float z2 = s._radius * cos(theta + theat_inc);

            float x3 = s._radius * sin(theta) * cos(alpha + alpha_inc);
            float y3 = s._radius * sin(theta) * sin(alpha + alpha_inc);
            float z3 = s._radius * cos(theta);

            float x4 = s._radius * sin(theta + theat_inc) * cos(alpha + alpha_inc);
            float y4 = s._radius * sin(theta + theat_inc) * sin(alpha + alpha_inc);
            float z4 = s._radius * cos(theta + theat_inc);

            vector_push_back_7(*v, x1, y1, z1, s._r, s._g, s._b, 1.0);
            vector_push_back_7(*v, x2, y2, z2, s._r, s._g, s._b, 1.0);
            vector_push_back_7(*v, x3, y3, z3, s._r, s._g, s._b, 1.0);
            vector_push_back_7(*v, x3, y3, z3, s._r, s._g, s._b, 1.0);
            vector_push_back_7(*v, x2, y2, z2, s._r, s._g, s._b, 1.0);
            vector_push_back_7(*v, x4, y4, z4, s._r, s._g, s._b, 1.0);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    return v;
}
std::vector<float>* SphereRenderer::_constructLatitudesAndLongitudeVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

	float polygonIncrement = _getPolygonIncrement();
    float inc = float(2 * M_PI) / polygonIncrement;

    //---------------------------------------------------------------------------------
    // Longitudes
    //---------------------------------------------------------------------------------
    //std::list<float> alphas = { 0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165 };

    //for (float alpha : alphas)
    for (float alphas = 0.0f; alphas < 180.0f; alphas += 10.0f)
    {
        float alpha = glm::radians(alphas);

        for (float theta = 0; theta < float(2 * M_PI); theta += inc)
        {
            float x1 = 1.001 * s._radius * sin(theta) * cos(alpha);
            float y1 = 1.001 * s._radius * sin(theta) * sin(alpha);
            float z1 = 1.001 * s._radius * cos(theta);

            float x2 = 1.001 * s._radius * sin(theta + inc) * cos(alpha);
            float y2 = 1.001 * s._radius * sin(theta + inc) * sin(alpha);
            float z2 = 1.001 * s._radius * cos(theta + inc);

            float cMult;
            if (alpha == 0.0f)
                cMult = 0.8;
            else
                cMult = 0.9;

            vector_push_back_7(*v, x1, y1, z1, s._r*cMult, s._g*cMult, s._b*cMult, 1.0);
            vector_push_back_7(*v, x2, y2, z2, s._r*cMult, s._g*cMult, s._b*cMult, 1.0);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    //---------------------------------------------------------------------------------
    // Special Latitudes
    //---------------------------------------------------------------------------------
    std::list<float> thetas = { 0, 66.5, 23.5, 90, 90 + 23.5, 90 + 66.5 };          // 0 = +z axis, 90 is equator, 90+66.5 is antarctic circle

    for (float theta : thetas)
    {
        theta = glm::radians(theta);

        for (float alpha = 0; alpha < float(2 * M_PI); alpha += inc)
        {
            float x1 = 1.001 * s._radius * sin(theta) * cos(alpha);
            float y1 = 1.001 * s._radius * sin(theta) * sin(alpha);
            float z1 = 1.001 * s._radius * cos(theta);

            float x2 = 1.001 * s._radius * sin(theta) * cos(alpha + inc);
            float y2 = 1.001 * s._radius * sin(theta) * sin(alpha + inc);
            float z2 = 1.001 * s._radius * cos(theta);

            vector_push_back_7(*v, x1, y1, z1, s._r*0.9, s._g*0.5, s._b*0.5, 1.0);
            vector_push_back_7(*v, x2, y2, z2, s._r*0.9, s._g*0.5, s._b*0.5, 1.0);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    //---------------------------------------------------------------------------------
    // Latitudes
    //---------------------------------------------------------------------------------
    for (float thetas = 0; thetas < 180; thetas += 10)
    {
        float theta = glm::radians(thetas);
        float cMult = 0.9;

        for (float alpha = 0; alpha < float(2 * M_PI) - inc; alpha += inc)
        {
            float x1 = 1.001 * s._radius * sin(theta) * cos(alpha);
            float y1 = 1.001 * s._radius * sin(theta) * sin(alpha);
            float z1 = 1.001 * s._radius * cos(theta);

            float x2 = 1.001 * s._radius * sin(theta) * cos(alpha + inc);
            float y2 = 1.001 * s._radius * sin(theta) * sin(alpha + inc);
            float z2 = 1.001 * s._radius * cos(theta);

            vector_push_back_7(*v, x1, y1, z1, s._r*cMult, s._g*cMult, s._b*cMult, 1.0);
            vector_push_back_7(*v, x2, y2, z2, s._r*cMult, s._g*cMult, s._b*cMult, 1.0);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    return v;
}


// Construct the circular/elliptical orbit
std::vector<float>* SphereRenderer::_constructOrbit()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;
    glm::vec3& color = s._orbitalPlaneColor;

    float alpha_inc = float(2 * M_PI) / 500;

    //---------------------------------------------------------------------------------
    // Orbit itself
    //---------------------------------------------------------------------------------
    for (float alpha = 0; alpha < 2 * M_PI; alpha += alpha_inc)
    {
        float x1 = s._orbitalRadius * cos(alpha);
        float y1 = s._orbitalRadius * sin(alpha);
        float z1 = 0;

        float x2 = s._orbitalRadius * cos(alpha + alpha_inc);
        float y2 = s._orbitalRadius * sin(alpha + alpha_inc);
        float z2 = 0;

        vector_push_back_7(*v, x1, y1, z1, color.r, color.g, color.b, 0.8);
        vector_push_back_7(*v, x2, y2, z2, color.r, color.g, color.b, 0.8);
    }

    return v;
}

std::vector<float>* SphereRenderer::_constructOrbitalPlaneVertices()
{
    std::vector<float>* v   = new std::vector<float>();
    Sphere& s               = _sphere;
    float m                 = 0.3;                          // color multiplier
    float radius            = s._orbitalRadius;
    glm::vec3& color        = s._orbitalPlaneColor;

    //---------------------------------------------------------------------------------
    // Orbital plane.  This is centered at origin.
    //---------------------------------------------------------------------------------
    vector_push_back_7(*v, -radius * 1.2, -radius * 1.2, 0, color.r*m, color.g*m, color.b*m, 1.0);
    vector_push_back_7(*v, +radius * 1.2, -radius * 1.2, 0, color.r*m, color.g*m, color.b*m, 1.0);
    vector_push_back_7(*v, +radius * 1.2, +radius * 1.2, 0, color.r*m, color.g*m, color.b*m, 1.0);
    vector_push_back_7(*v, +radius * 1.2, +radius * 1.2, 0, color.r*m, color.g*m, color.b*m, 1.0);
    vector_push_back_7(*v, -radius * 1.2, +radius * 1.2, 0, color.r*m, color.g*m, color.b*m, 1.0);
    vector_push_back_7(*v, -radius * 1.2, -radius * 1.2, 0, color.r*m, color.g*m, color.b*m, 1.0);

    return v;
}
std::vector<float>* SphereRenderer::_constructOrbitalPlaneGridVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

    //---------------------------------------------------------------------------------
    // Orbital plane grid.
    //---------------------------------------------------------------------------------
    // generate parallel lines along Y axis in the orbital plane
    //float inc = float(_orbitalRadius) / int(_orbitalRadius / 50.0);

    float maxGridLines      = 20;
    float inc               = (s._orbitalRadius * 2 * 1.2) / maxGridLines;
    //inc = std::max(inc, 50.0f);
    printf("inc = %f\n", inc);

    float x, y;
    float m                 = 0.4;      // color multiplier
    float radius            = s._orbitalRadius;
    glm::vec3& color        = s._orbitalPlaneColor;

    x = y = -radius * 1.2;

    for (int i = 0; i <= maxGridLines; i++)
    {
        vector_push_back_7(*v, x, -radius * 1.2, +1, color.r*m, color.g*m, color.b*m, 1.0);
        vector_push_back_7(*v, x, +radius * 1.2, +1, color.r*m, color.g*m, color.b*m, 1.0);
        vector_push_back_7(*v, x, -radius * 1.2, -1, color.r*m, color.g*m, color.b*m, 1.0);
        vector_push_back_7(*v, x, +radius * 1.2, -1, color.r*m, color.g*m, color.b*m, 1.0);

        x += inc;
    }
    for (int i = 0; i <= maxGridLines; i++)
    {
        vector_push_back_7(*v, -radius * 1.2, y, +1, color.r*m, color.g*m, color.b*m, 1.0);
        vector_push_back_7(*v, +radius * 1.2, y, +1, color.r*m, color.g*m, color.b*m, 1.0);
        vector_push_back_7(*v, -radius * 1.2, y, -1, color.r*m, color.g*m, color.b*m, 1.0);
        vector_push_back_7(*v, +radius * 1.2, y, -1, color.r*m, color.g*m, color.b*m, 1.0);

        y += inc;
    }

    return v;
}


void SphereRenderer::constructVerticesAndSendToGpu(OglHandles oglHandles)
{
    GLuint vbo;
    std::vector<float> *v = nullptr;

    //---------------------------------------------------------------------------------------------------
    // The sphere itself
    v = _constructMainSphereVertices();

    glGenVertexArrays(1, &_mainVao);
    glBindVertexArray(_mainVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    numMainSphereVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;

    //---------------------------------------------------------------------------------------------------
    // latitude and longitude
    v = _constructLatitudesAndLongitudeVertices();

    glGenVertexArrays(1, &_latAndLongVao);
    glBindVertexArray(_latAndLongVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    numLatAndLongVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;

    //---------------------------------------------------------------------------------------------------
    // Orbital plane
    v = _constructOrbitalPlaneVertices();

    glGenVertexArrays(1, &_orbitalPlaneVao);
    glBindVertexArray(_orbitalPlaneVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    numOrbitalPlaneVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;

    //---------------------------------------------------------------------------------------------------
    // orbital plane grid lines
    v = _constructOrbitalPlaneGridVertices();

    glGenVertexArrays(1, &_orbitalPlaneGridVao);
    glBindVertexArray(_orbitalPlaneGridVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    numOrbitalPlaneGridVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;

    //---------------------------------------------------------------------------------------------------
    // Orbital itself
    v = _constructOrbit();

    glGenVertexArrays(1, &_orbitVao);
    glBindVertexArray(_orbitVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    numOrbitVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;


}

void SphereRenderer::render(OglHandles oglHandles, Sphere* otherSphere)
{
    //----------------------------------------------------------------------------------------------------
    // Main sphere
    //----------------------------------------------------------------------------------------------------
    glUniformMatrix4fv(
        oglHandles.uniModel,
        1,
        GL_FALSE,
        glm::value_ptr(_sphere.getModelMatrix())
    );

    glUniform1i(oglHandles.uniMyIsLightSource, _bIsLightSource);
    glUniform3fv(oglHandles.uniMyCenterTransformed, 1, glm::value_ptr(_sphere.getCenter()));
    glUniform1f(oglHandles.uniMyRadius, _sphere.getRadius());
    glUniform1i(oglHandles.uniMyIsValud, true);
    glUniform1f(oglHandles.uniNightColorMultiplier, _nightColorMultiplier);
    //glEnable(GL_MULTISAMPLE);

    if (otherSphere != nullptr)
    {
        // When drawing earth, other sphere is moon.
        glUniform1f(oglHandles.uniOtherSphereRadius, otherSphere->getRadius());
        glUniform3fv(
            oglHandles.uniOtherSphereCenterTransformed,
            1,
            glm::value_ptr(otherSphere->getModelTransformedCenter())
        );
    }

    glBindVertexArray(_mainVao);

    // Draw vertices
    glDrawArrays(GL_TRIANGLES, 0, numMainSphereVertices);

    //----------------------------------------------------------------------------------------------------
    // Latitude and longitude
    //----------------------------------------------------------------------------------------------------
    if (bShowLatitudesAndLongitudes)
    {
        glBindVertexArray(_latAndLongVao);

        // Draw vertices
        glDrawArrays(GL_LINES, 0, numLatAndLongVertices);
    }


    //----------------------------------------------------------------------------------------------------
    // Orbital plane
    //----------------------------------------------------------------------------------------------------
    // orbital plane has its own model transform different from the sphere itself.
    if (bShowOrbitalPlane)
    {
        glUniformMatrix4fv(
            oglHandles.uniModel,
            1,
            GL_FALSE,
            glm::value_ptr(_sphere.getOrbitalPlaneModelMatrix())
        );

        glUniform1i(oglHandles.uniMyIsValud, false);


        // Draw orbital plane grid
        glBindVertexArray(_orbitalPlaneGridVao);
        glDrawArrays(GL_LINES, 0, numOrbitalPlaneGridVertices);

        // Draw vertices
//        glDepthMask(GL_FALSE);
        glBindVertexArray(_orbitalPlaneVao);
        glDrawArrays(GL_TRIANGLES, 0, numOrbitalPlaneVertices);
//        glDepthMask(GL_TRUE);


    }

    //----------------------------------------------------------------------------------------------------
    // Orbital itself
    //----------------------------------------------------------------------------------------------------
    // Uses same transform as that of orbital plane. But orbital plane may not be drawn depending on flags. Hence
    // set the model matrix again.
    if (bShowOrbit)
    {
        glUniformMatrix4fv(
            oglHandles.uniModel,
            1,
            GL_FALSE,
            glm::value_ptr(_sphere.getOrbitalPlaneModelMatrix())
        );

        glUniform1i(oglHandles.uniMyIsValud, false);

        glBindVertexArray(_orbitVao);
        glDrawArrays(GL_LINES, 0, numOrbitVertices);
    }

}

float SphereRenderer::_getPolygonIncrement()
{
	switch (_polygonCountLevel)
	{
	case PolygonCountLevel_Low:
		return 100.0;
	case PolygonCountLevel_Medium:
		return 500.0;
	case PolygonCountLevel_High:
		return 1800.0;
	}
}