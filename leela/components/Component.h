#pragma once

#include "GlslProgram.h"


//
// A component adds to the functionality of a scene object.
//  - A subclassed component typically has a handle to the scene object (subclass of SceneObject).
//  - On each advance(), the component can examine the scene object and derive other information.
// 
// TODO examples.
//
class Component
{
public:
	Component(std::string componentName)
	{
		_componentName = componentName;
	}

	virtual void init() = 0;
	virtual void advance(float stepMultiplier) = 0;


	std::string getComponentName()
	{
		return _componentName;
	}

private:
	std::string _componentName;
};