#pragma once

#include <typeinfo>
#include <vector>
#include <string>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Renderer.h>

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

    // subclass of SceneObject is expected to examine _sceneParent in order to typecast it and save it as a pointer to the desired subclass.
    //  - For e.g. SphericalBody class might want to know if _sceneParent is another SphericalBody instance, and use it later.
    virtual void parentChanged() {}

    // transform to move this scene object from its parent's position
    virtual glm::mat4 getPositionTransform();

    // transform to rotate/orient this object in its local coordinate system
    virtual glm::mat4 getOrientationTransform();

    // combined transform
    glm::mat4 getTransform();

    void addComponent(Component* component);
    void removeComponent(Component* component);

    // Add a child scene object
    void addSceneObject(SceneObject* childObject);
    void _setParent(SceneObject* parent);
    static void _printIndent(int indent);
    static void printTree(SceneObject* obj, int indent = 0);
    static SceneObject* getSceneObjectByName(SceneObject* obj, std::string name);
    Component* getComponentByName(std::string name);

    bool hidden();
    void setHidden(bool hide = true);

public:

    std::vector<SceneObject*> _childSceneObjects;
    std::vector<Component*> _components;					// components of this scene object
    std::vector<Renderer*> _renderers;                      // subset of components that are renderers

    bool _hidden = false;                           // if hidden, none of its components will be processed.
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