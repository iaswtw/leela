#pragma once

#include <string>

class SceneObject;

//
// A component adds to the functionality of a scene object.
//  - A subclassed component typically has a handle to the scene object (subclass of SceneObject)
//    of which it is a component of.
//  - On each advance(), the component can examine the scene object and derive other information.
// 
// TODO examples.
//
class Component
{
public:

	Component(std::string componentName = "NoName")
		: _componentName(componentName)
	{}

	virtual void init() = 0;
	virtual void advance(float stepMultiplier) = 0;
	virtual void parentChanged() {}

	void setParent(SceneObject* parent)
	{
		_sceneParent = parent;
		parentChanged();
	}

	std::string getComponentName()
	{
		return _componentName;
	}

public:
	std::string _componentName;
	SceneObject* _sceneParent = nullptr;
};