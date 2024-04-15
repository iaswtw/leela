#pragma once


#include <vector>
#include <Component.h>

class SceneObject
{
public:
	SceneObject()
		: _parent(nullptr)
	{
	}
	
	SceneObject(SceneObject * parent)
		: _parent(parent)
	{	
	}

	void addComponent(Component* component)
	{
		_components.push_back(component);
	}

	void addChildSceneObject(SceneObject* sceneObject)
	{
		_childSceneObjects.push_back(sceneObject);
	}

public:

	SceneObject * _parent = nullptr;
	std::vector<SceneObject*> _childSceneObjects;
	std::vector<Component*> _components;					// components of this scene object

};