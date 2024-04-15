#pragma once

#include "GlslProgram.h"

// All known component types
typedef enum
{
	ComponentType_Body,
	ComponentType_OrbitalPlane,
	ComponentType_OrbitalPlaneGrid,
	ComponentType_LatitudesLongitudes,
	ComponentType_Axis,


} ComponentType;


class Component
{
public:
	Component(ComponentType type)
	{
		_componentType = type;
	}

	virtual void init() = 0;
	virtual void advance(float stepMultiplier) = 0;
	virtual void render(GlslProgram& glslProgram) = 0;
	virtual void renderTransparent() {}

	ComponentType getComponentType()
	{
		return _componentType;
	}

private:
	ComponentType _componentType;
};