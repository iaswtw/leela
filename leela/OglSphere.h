#pragma once


#include "sphere.h"

#include <GL/glew.h>

class OglSphere
{
public:
    OglSphere(Sphere & sphere) :
        _sphere(sphere)
    {
    }
    
    ~OglSphere()
    {
    }


private:

    GLuint sphereVao;

    Sphere & _sphere;
};