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

	virtual void advance(float stepMultiplier) = 0;

	// transform to move this scene object from its parent's position
	virtual glm::mat4 getPositionTransform()
	{ 
		return glm::mat4(1.0f);
	}

	// transform to rotate/orient this object in its local coordinate system
	virtual glm::mat4 getOrientationTransform()
	{
		return glm::mat4(1.0f); 
	}

	// combined transform
	glm::mat4 getTransform()
	{
		return getPositionTransform() * getOrientationTransform();
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

class Scene : public SceneObject
{
	void advance(float stepMultiplier) {}
};