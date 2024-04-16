#pragma once

#include <typeinfo>
#include <vector>
#include <Component.h>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class SceneObject
{
public:
	SceneObject(SceneObject * parent = nullptr, std::string name = "NoName")
		: _parent(parent),
		  _name(name)
	{	
	}

	virtual void init() = 0;
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

	static void _printIndent(int indent)
	{
		for (int i = 0; i < indent; i++)
			printf(" ");
	}

	static void printTree(SceneObject * obj, int indent = 0)
	{
		_printIndent(indent);
		printf("%s:\n", obj->_name.c_str());

		indent += 4;
		for (Component* c : obj->_components)
		{
			_printIndent(indent);
			printf("%s\n", typeid(*c).name());
		}

		for (SceneObject* o : obj->_childSceneObjects)
		{
			printTree(o, indent);
		}
	}

public:

	std::vector<SceneObject*> _childSceneObjects;
	std::vector<Component*> _components;					// components of this scene object

	SceneObject * _parent = nullptr;
	std::string _name;
};

class Scene : public SceneObject
{
	void init() {}
	void advance(float stepMultiplier) {}
};