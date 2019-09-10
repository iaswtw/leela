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
    void createVaoAndVbos(OglHandles oglHandles);
    void render(OglHandles oglHandles, Sphere* otherSphere = nullptr);

public:
    bool bShowOrbitalPlane = false;
    bool bShowOrbit = true;
    bool bShowLatitudesAndLongitudes = false;

private:
    Sphere& _sphere;

    GLuint _mainVao;
    GLuint _orbitalPlaneVao;
    GLuint _orbitalPlaneGridVao;            // grid lines in the orbital plane
    GLuint _orbitVao;
    GLuint _latAndLongVao;

    bool _bIsLightSource = false;
};

