#pragma once

#include <typeinfo>
#include <vector>
#include <string>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Component;

class SceneObject
{
public:
    SceneObject(std::string name = "NoName")
        : _name(name)
    {	
    }

    virtual void init() = 0;
    virtual void advance(float stepMultiplier) = 0;

    // subclass is expected to examine _sceneParent in order to typecast and save as a point to a subclass.
    // For e.g. SphericalBody class might want to know if _sceneParent is another SphericalBody instance, and use it later.
    virtual void parentChanged() {}

    // transform to move this scene object from its parent's position
    virtual glm::mat4 getPositionTransform();

    // transform to rotate/orient this object in its local coordinate system
    virtual glm::mat4 getOrientationTransform();

    // combined transform
    glm::mat4 getTransform();

    void addComponent(Component* component);

    // Add a child scene object
    void addSceneObject(SceneObject* childObject);
    void _setParent(SceneObject* parent);
    static void _printIndent(int indent);
    static void printTree(SceneObject* obj, int indent = 0);

public:

    std::vector<SceneObject*> _childSceneObjects;
    std::vector<Component*> _components;					// components of this scene object

    SceneObject * _sceneParent = nullptr;
    std::string _name;
};



//
// A comcrete SceneObject intended to be used as the top-level scene object.
//
class Scene : public SceneObject
{
    void init() {}
    void advance(float stepMultiplier) {}
};