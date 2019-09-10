#pragma once

#include "sphere.h"

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "OglHandles.h"

class SphereRenderer
{
public:
    SphereRenderer(Sphere& sphere);
    ~SphereRenderer();

    void setAsLightSource();
    void constructVerticesAndSendToGpu(OglHandles oglHandles);
    void render(OglHandles oglHandles, Sphere* otherSphere = nullptr);

public:
    bool bShowOrbitalPlane = false;
    bool bShowOrbit = true;
    bool bShowLatitudesAndLongitudes = false;


private:
    std::vector<float>* _constructMainSphereVertices();
    std::vector<float>* _constructLatitudesAndLongitudeVertices();
    std::vector<float>* _constructOrbit();
    std::vector<float>* _constructOrbitalPlaneVertices();
    std::vector<float>* _constructOrbitalPlaneGridVertices();

private:
    Sphere& _sphere;

    GLuint _mainVao;
    GLuint _orbitalPlaneVao;
    GLuint _orbitalPlaneGridVao;            // grid lines in the orbital plane
    GLuint _orbitVao;
    GLuint _latAndLongVao;

    int numMainSphereVertices = -1;
    int numLatAndLongVertices = -1;
    int numOrbitalPlaneVertices = -1;
    int numOrbitalPlaneGridVertices = -1;
    int numOrbitVertices = -1;


    bool _bIsLightSource = false;
};

