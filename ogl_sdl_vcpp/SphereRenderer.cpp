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

SphereRenderer::SphereRenderer(Sphere& sphere) :
    _sphere(sphere)
{
}


SphereRenderer::~SphereRenderer()
{
}

void SphereRenderer::setAsLightSource()
{
    _bIsLightSource = True;
}

void SphereRenderer::createVaoAndVbos(OglHandles oglHandles)
{
    GLuint vbo;

    //---------------------------------------------------------------------------------------------------
    // The sphere itself
    glGenVertexArrays(1, &_mainVao);
    glBindVertexArray(_mainVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * _sphere.getVertices().size(),
        _sphere.getVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------
    // latitude and longitude
    glGenVertexArrays(1, &_latAndLongVao);
    glBindVertexArray(_latAndLongVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * _sphere.getLatitudeAndLongitudeVertices().size(),
        _sphere.getLatitudeAndLongitudeVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Orbital plane
    glGenVertexArrays(1, &_orbitalPlaneVao);
    glBindVertexArray(_orbitalPlaneVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * _sphere.getOrbitalPlaneVertices().size(),
        _sphere.getOrbitalPlaneVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Orbital itself
    glGenVertexArrays(1, &_orbitVao);
    glBindVertexArray(_orbitVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * _sphere.getOrbitVertices().size(),
        _sphere.getOrbitVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);


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
    glDrawArrays(GL_TRIANGLES, 0, _sphere.getVertices().size() / VERTEX_STRIDE_IN_VBO);

    //----------------------------------------------------------------------------------------------------
    // Latitude and longitude
    //----------------------------------------------------------------------------------------------------
    if (bShowLatitudesAndLongitudes)
    {
        glBindVertexArray(_latAndLongVao);

        // Draw vertices
        glDrawArrays(GL_LINES, 0, _sphere.getLatitudeAndLongitudeVertices().size() / VERTEX_STRIDE_IN_VBO);
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

        glBindVertexArray(_orbitalPlaneVao);

        // Draw vertices
//        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, _sphere.getOrbitalPlaneVertices().size() / VERTEX_STRIDE_IN_VBO);
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
        glDrawArrays(GL_LINES, 0, _sphere.getOrbitVertices().size() / VERTEX_STRIDE_IN_VBO);
    }

}