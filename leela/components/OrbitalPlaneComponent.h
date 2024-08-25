#pragma once

#include <Component.h>
#include <Sphere.h>
#include <Utils.h>

class OrbitalPlaneComponent : public Component
{
public:
	OrbitalPlaneComponent()
		: Component(ComponentType_OrbitalPlane)
	{
	}

    void init()
    {
        constructOrbitalPlaneVertices();
        constructOrbitalPlaneGridVertices();
    }



    void advance(float stepMultiplier)
    {

    }



public:





    Sphere * _sphere = nullptr;
};